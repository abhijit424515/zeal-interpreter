#ifndef NODE_HH
#define NODE_HH

#include "scope.hh"

enum class PrefixOp;
enum class InfixOp;

extern bool repl;
extern EnvStack* env_stack;
extern unordered_map<ObjType,string> objtype_str;
extern unordered_map<PrefixOp,string> prefix_str;
extern unordered_map<InfixOp,string> infix_str;

enum class NodeType {
	PROG,
	STMT,
	BLOCK,
	EXPR,
	FUNC,
};

enum class StmtType {
	LET,
	ASG,
	RETURN,
	IF,
	EXP,
};

enum class ExpType {
	CONST,
	ID,
	PREFIX,
	INFIX,
	CALL,
};

enum class ConstType {
	INT,
	FLT,
	STR,
	BOOL,
	NONE,
	FN,
};

enum class PrefixOp {
	NEG,
	NOT,
};

enum class InfixOp {
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
	EQ,
	NE,
	LT,
	LE,
	GT,
	GE,
	AND,
	OR,
};

struct Node {
	NodeType ntype;
	virtual void code() = 0;
};

// --------------------------------

struct Statement: Node {
	StmtType stype;
	virtual void code() = 0;
};

struct Expression: Node {
	ExpType etype;
	Object* value;
	virtual void code() = 0;
};

struct StmtWrapper {
	vector<Statement*> stmts;

	StmtWrapper() {}
	StmtWrapper(Statement* stmt) {
		stmts.push_back(stmt);
	}
	void push_back(Statement* stmt) {
		stmts.push_back(stmt);
	}
	~StmtWrapper() {
		for (auto stmt : stmts) delete stmt;
	}
};

struct ArgWrapper {
	vector<string*> args;

	ArgWrapper() {}
	ArgWrapper(string* arg) {
		args.push_back(arg);
	}
	void push_back(string* arg) {
		args.push_back(arg);
	}
	~ArgWrapper() {
		for (auto arg : args) delete arg;
	}
};

struct ExpWrapper {
	vector<Expression*> exps;

	ExpWrapper() {}
	ExpWrapper(Expression* exp) {
		exps.push_back(exp);
	}
	void push_back(Expression* exp) {
		exps.push_back(exp);
	}
	~ExpWrapper() {
		for (auto exp : exps) delete exp;
	}
};

// --------------------------------

struct BlockStmt: Node {
	StmtWrapper *stmt_list;

	BlockStmt(StmtWrapper *s): stmt_list(s) { ntype = NodeType::BLOCK; }
	~BlockStmt() { delete stmt_list; }
	void code() override {
		for (auto stmt : stmt_list->stmts) stmt->code();
	}
};

struct Program: Node {
	StmtWrapper *stmt_list;

	Program() { ntype = NodeType::PROG; }
	Program(StmtWrapper *s): stmt_list(s) { ntype = NodeType::PROG; }
	~Program() { delete stmt_list; }
	void code() override {
		for (auto stmt : stmt_list->stmts) stmt->code();
	}
};

// --------------------------------

struct Fn: Object {
	ArgWrapper* params;
	BlockStmt* body;

	Fn(ArgWrapper* p, BlockStmt* b): params(p), body(b) { otype = ObjType::FN; }
	~Fn() { delete params, body; }
	string str() const override {
		string s = "fn(";
		for (auto p: params->args) s += *p + ",";
		s.pop_back();
		s += ");";
		return s;
	}
};

// --------------------------------

struct LetStmt: Statement {
	string id;
	Expression* value;

	LetStmt(const string& n, Expression* e): id(n), value(e) { stype = StmtType::LET; }
	~LetStmt() { delete value; }
	void code() override {
		value->code();
		if (env_stack->redeclaration(id)) {
			value->value = new Error(ErrorType::REDECL, id);
			return;
		}
		env_stack->create(id, value->value);
	}
};

struct AsgStmt: Statement {
	string id;
	Expression* value;

	AsgStmt(const string& n, Expression* e): id(n), value(e) { stype = StmtType::ASG; }
	~AsgStmt() { delete value; }
	void code() override {
		value->code();
		if (env_stack->undefined(id)) {
			value->value = new Error(ErrorType::UNDEF, id);
			return;
		}
		env_stack->modify(id) = value->value;
	}
};

struct RetStmt: Statement {
	Expression* value;

	RetStmt(Expression* e): value(e) { stype = StmtType::RETURN; }
	~RetStmt() { delete value; }
	void code() override {
		value->code();
		env_stack->modify("return") = dup(value->value);
	}
};

struct IfStmt: Statement {
	Expression* cond;
	BlockStmt* then;
	BlockStmt* els;

	IfStmt(Expression* c, BlockStmt* t, BlockStmt* e): cond(c), then(t), els(e) { stype = StmtType::IF; }
	~IfStmt() {
		delete cond;
		delete then;
		delete els;
	}
	void code() override {
		cond->code();
		if (cond->value->otype != ObjType::BOOL) {
			cond->value = new Error(ErrorType::TYPE, "if condition must be a boolean");
			return;
		}
		if (*(bool*)(cond->value->value)) then->code();
		else els->code();
	}
};

struct ExpStmt: Statement {
	Expression* value;

	ExpStmt(Expression* e): value(e) { stype = StmtType::EXP; }
	~ExpStmt() { delete value; }
	void code() override {
		value->code();
		repl_print(value->value->str());
	}
};

// --------------------------------

struct Call: Expression {
	string id;
	ExpWrapper* args;
	Scope* call_scope;

	Call(const string& n, ExpWrapper* a): id(n), args(a) { etype = ExpType::CALL; }
	~Call() { delete args; }
	void code() override {
		if (env_stack->undefined(id)) {
			value = new Error(ErrorType::UNDEF, id);
			return;
		}
		Object* obj = env_stack->modify(id);
		if (obj->otype != ObjType::FN) {
			value = new Error(ErrorType::TYPE, id);
			return;
		}
		Fn* fn = (Fn*)obj;

		// check for argument count
		if (fn->params->args.size() != args->exps.size()) {
			value = new Error(ErrorType::ARG, id + " expects " + to_string(fn->params->args.size()) + " arguments");
			return;
		}

		// create a new scope and push it to the env stack
		call_scope = new Scope();
		env_stack->stack.push_back(call_scope);

		// bind the return value to the local scope
		value = new Null();
		env_stack->create("return", value);

		// evaluate the arg expressions, then bind them to the local scope 
		for (int i = 0; i < args->exps.size(); i++) {
			args->exps[i]->code();
			string name = *(fn->params->args[i]);

			if (env_stack->redeclaration(name)) {
				value = new Error(ErrorType::REDECL, name);
				return;
			}

			// copy the value object, and bind it to the local scope
			call_scope->insert({name, dup(args->exps[i]->value)});
		}

		// execute the function body 
		for (int i = 0; i < fn->body->stmt_list->stmts.size(); i++)
			fn->body->stmt_list->stmts[i]->code();

		// retrieve return value from the local scope, and duplicate it
		value = dup(env_stack->modify("return"));

		// pop the scope from the env stack
		env_stack->pop();
	}
};

struct Const: Expression {
	ConstType ctype;

	Const(string v, ConstType t): ctype(t) {
		etype = ExpType::CONST;
		switch (t) {
			case ConstType::INT: value = new Int(stoi(v)); break;
			case ConstType::FLT: value = new Double(stof(v)); break;
			case ConstType::STR: value = new String(v); break;
			default: 
				cerr << "[error] Const::Const(string v, ConstType t)" << endl;
				exit(1);
		}
	}
	Const(): ctype(ConstType::NONE) {
		etype = ExpType::CONST;
		value = new Null();
	}
	Const(bool v): ctype(ConstType::BOOL) {
		etype = ExpType::CONST;
		value = new Bool(v);
	}
	Const(ArgWrapper* p, BlockStmt* b): ctype(ConstType::FN) {
		etype = ExpType::CONST;
		value = new Fn(p, b);
	}
	~Const() {
		switch (ctype) {
			case ConstType::INT: delete (Int*)value; break;
			case ConstType::FLT: delete (Double*)value; break;
			case ConstType::STR: delete (String*)value; break;
			case ConstType::BOOL: delete (Bool*)value; break;
			case ConstType::NONE: delete (Null*)value; break;
			case ConstType::FN: delete (Fn*)value; break;
			default: 
				cerr << "[error] Const::~Const()" << endl;
				exit(1);
		}
	}
	void code() override {}
};

struct Idf : Expression {
	string name;

	Idf(const string& n): name(n) { etype = ExpType::ID; }
	void code() override {
		if (env_stack->undefined(name)) 
			value = new Error(ErrorType::UNDEF, name);
		else value = env_stack->modify(name);
	}
};

struct PrefixExp : Expression {
	PrefixOp op;
	Expression* right;

	PrefixExp(PrefixOp o, Expression* r): op(o), right(r) { etype = ExpType::PREFIX; }
	~PrefixExp() { delete right; }
	void code() override {
		right->code();
		switch (op) {
			case PrefixOp::NEG:
				if (right->value->otype == ObjType::INT)
					value = new Int(-(*(int*)(right->value->value)));
				else if (right->value->otype == ObjType::FLT)
					value = new Double(-*(double*)(right->value->value));
				else
					value = new Error(ErrorType::UNSOP, prefix_str[op] + right->value->str());
				break;
			case PrefixOp::NOT:
				if (right->value->otype != ObjType::BOOL)
					value = new Error(ErrorType::UNSOP, prefix_str[op] + right->value->str());
				else 
					value = new Bool(!*((bool*)right->value->value));
				break;
			default:
				cerr << "[error] PrefixExp::PrefixExp(PrefixOp o, Expression* r)" << endl;
				exit(1);
		}
	}
};

struct InfixExp : Expression {
	Expression* left;
	InfixOp op;
	Expression* right;

	InfixExp(Expression* l, InfixOp o, Expression* r): left(l), op(o), right(r) { etype = ExpType::INFIX; }
	~InfixExp() { delete left, right; }
	void code() override {
		left->code();
		right->code();
		void *lv = left->value->value, *rv = right->value->value;
		if (left->value->otype == right->value->otype) {
			ObjType t = left->value->otype;
			bool v = false;
			bool inv = false;

			switch (op) {
				case InfixOp::ADD:
					if (t == ObjType::INT) value = new Int(*(int*)lv + *(int*)rv);
					else if (t == ObjType::FLT) value = new Double(*(double*)lv + *(double*)rv);
					else if (t == ObjType::STR) value = new String(*(string*)lv + *(string*)rv);
					else inv = true;
					break;
				case InfixOp::SUB:
					if (t == ObjType::INT) value = new Int(*(int*)lv - *(int*)rv);
					else if (t == ObjType::FLT) value = new Double(*(double*)lv - *(double*)rv);
					else inv = true;
					break;
				case InfixOp::MUL:
					if (t == ObjType::INT) value = new Int(*(int*)lv * *(int*)rv);
					else if (t == ObjType::FLT) value = new Double(*(double*)lv * *(double*)rv);
					else inv = true;
					break;
				case InfixOp::DIV:
					if (t == ObjType::INT) value = new Int(*(int*)lv / *(int*)rv);
					else if (t == ObjType::FLT) value = new Double(*(double*)lv / *(double*)rv);
					else inv = true;
					break;
				case InfixOp::MOD:
					if (t == ObjType::INT) value = new Int(*(int*)lv % *(int*)rv);
					else inv = true;
					break;
				case InfixOp::EQ:
					if (t == ObjType::INT) v = *(int*)lv == *(int*)rv;
					else if (t == ObjType::FLT) v = *(double*)lv == *(double*)rv;
					else if (t == ObjType::STR) v = *(string*)lv == *(string*)rv;
					else if (t == ObjType::BOOL) v = *(bool*)lv == *(bool*)rv;
					else if (t == ObjType::NONE) v = true;
					else inv = true;
					if (!inv) value = new Bool(v);
					break;
				case InfixOp::NE:
					if (t == ObjType::INT) v = *(int*)lv != *(int*)rv;
					else if (t == ObjType::FLT) v = *(double*)lv != *(double*)rv;
					else if (t == ObjType::STR) v = *(string*)lv != *(string*)rv;
					else if (t == ObjType::BOOL) v = *(bool*)lv != *(bool*)rv;
					else if (t == ObjType::NONE) v = false;
					else inv = true;

					if (!inv) value = new Bool(v);
					break;
				case InfixOp::LT:
					if (t == ObjType::INT) v = *(int*)lv < *(int*)rv;
					else if (t == ObjType::FLT) v = *(double*)lv < *(double*)rv;
					else inv = true;
				
					if (!inv) value = new Bool(v);
					break;
				case InfixOp::LE:
					if (t == ObjType::INT) v = *(int*)lv <= *(int*)rv;
					else if (t == ObjType::FLT) v = *(double*)lv <= *(double*)rv;
					else inv = true;

					if (!inv) value = new Bool(v);
					break;
				case InfixOp::GT:
					if (t == ObjType::INT) v = *(int*)lv > *(int*)rv;
					else if (t == ObjType::FLT) v = *(double*)lv > *(double*)rv;
					else inv = true;

					if (!inv) value = new Bool(v);
					break;
				case InfixOp::GE:
					if (t == ObjType::INT) v = *(int*)lv >= *(int*)rv;
					else if (t == ObjType::FLT)	v = *(double*)lv >= *(double*)rv;
					else inv = true;

					if (!inv) value = new Bool(v);
					break;
				case InfixOp::AND:
					if (t == ObjType::BOOL) value = new Bool(*(bool*)lv && *(bool*)rv);
					else inv = true;
					break;
				case InfixOp::OR:
					if (t == ObjType::BOOL) value = new Bool(*(bool*)lv || *(bool*)rv);
					else inv = true;
					break;
				default:
					cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { default }" << endl;
					exit(1);
			}
			if (inv) value = new Error(ErrorType::UNSOP, left->value->str() + infix_str[op] + right->value->str());
		} else value = new Error(ErrorType::TYPE, objtype_str[left->value->otype] + infix_str[op] + objtype_str[right->value->otype]);
	}
};

#endif