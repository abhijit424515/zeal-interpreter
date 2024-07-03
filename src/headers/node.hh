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
};

// --------------------------------

struct Statement: Node {
	StmtType stype;
	virtual void code() = 0;
};

struct Expression: Node {
	ExpType etype;
	virtual unique_ptr<Object> code() = 0;
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
	void code() { // [TODO] may need some changes
		for (auto stmt : stmt_list->stmts) stmt->code();
	}
};

struct Program: Node {
	StmtWrapper *stmt_list;

	Program() { ntype = NodeType::PROG; }
	Program(StmtWrapper *s): stmt_list(s) { ntype = NodeType::PROG; }
	~Program() { delete stmt_list; }
	void code() { // [TODO] may need some changes
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
	Expression* rhs;

	LetStmt(const string& n, Expression* e): id(n), rhs(e) { stype = StmtType::LET; }
	~LetStmt() { delete rhs; }
	void code() override {
		unique_ptr<Object> v = move(rhs->code());
		if (env_stack->redeclaration(id)) {
			v = make_unique<Error>(ErrorType::REDECL, id);
			return;
		}
		env_stack->insert(id, move(v));
	}
};

struct AsgStmt: Statement {
	string id;
	Expression* rhs;

	AsgStmt(const string& n, Expression* e): id(n), rhs(e) { stype = StmtType::ASG; }
	~AsgStmt() { delete rhs; }
	void code() override {
		unique_ptr<Object> v = move(rhs->code());
		if (env_stack->undefined(id)) {
			v = make_unique<Error>(ErrorType::UNDEF, id);
			return;
		}
		env_stack->insert(id, move(v));
	}
};

struct RetStmt: Statement {
	Expression* value;

	RetStmt(Expression* e): value(e) { stype = StmtType::RETURN; }
	~RetStmt() { delete value; }
	void code() override {
		unique_ptr<Object> v = move(value->code());
		env_stack->insert("return", move(v));
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
		unique_ptr<Object> v = move(cond->code());
		if (v->otype != ObjType::BOOL) {
			v = make_unique<Error>(ErrorType::TYPE, "if condition must be a boolean");
			return;
		} else { // [TODO] how to propagate error object ?
			if (*(bool*)(v->value)) then->code();
			else els->code();
		}
	}
};

struct ExpStmt: Statement {
	Expression* value;

	ExpStmt(Expression* e): value(e) { stype = StmtType::EXP; }
	~ExpStmt() { delete value; }
	void code() override {
		unique_ptr<Object> v = move(value->code());
		repl_print(v->str());
	}
};

// --------------------------------

struct Call: Expression {
	string id;
	ExpWrapper* args;

	Call(const string& n, ExpWrapper* a): id(n), args(a) { etype = ExpType::CALL; }
	~Call() { delete args; }
	unique_ptr<Object> code() override {
		unique_ptr<Object> value;

		if (env_stack->undefined(id))
			return make_unique<Error>(ErrorType::UNDEF, id);

		unique_ptr<Object> obj = env_stack->get(id);
		if (obj->otype != ObjType::FN)
			return make_unique<Error>(ErrorType::TYPE, id);

		unique_ptr<Fn> fn = static_unique_ptr_cast<Fn>(move(obj));
		if (fn->params->args.size() != args->exps.size())
			return make_unique<Error>(ErrorType::ARG, id + " expects " + to_string(fn->params->args.size()) + " arguments");

		// create a new scope for the function call
		env_stack->push_scope();

		// bind the return value to the local scope
		value = make_unique<Null>();
		env_stack->insert("return", move(value));

		// evaluate the arg expressions, then bind them to the local scope 
		for (int i = 0; i < args->exps.size(); i++) {
			string name = *(fn->params->args[i]);
			if (env_stack->redeclaration(name))
				return make_unique<Error>(ErrorType::REDECL, name);

			unique_ptr<Object> v = move(args->exps[i]->code());
			env_stack->insert(name, move(v));
		}

		// execute the function body 
		for (int i = 0; i < fn->body->stmt_list->stmts.size(); i++)
			fn->body->stmt_list->stmts[i]->code();

		// retrieve return value from the local scope before popping it
		value = env_stack->get("return");
		env_stack->pop_scope();
		return move(value);
	}
};

struct Const: Expression {
	ConstType ctype;
	unique_ptr<Object> cv;

	Const(string v, ConstType t): ctype(t) {
		etype = ExpType::CONST;
		switch (t) {
			case ConstType::INT: cv = make_unique<Int>(stoi(v)); break;
			case ConstType::FLT: cv = make_unique<Double>(stof(v)); break;
			case ConstType::STR: cv = make_unique<String>(v); break;
			default: 
				cerr << "[error] Const::Const(string v, ConstType t)" << endl;
				exit(1);
		}
	}
	Const(): ctype(ConstType::NONE) {
		etype = ExpType::CONST;
		cv = make_unique<Null>();
	}
	Const(bool v): ctype(ConstType::BOOL) {
		etype = ExpType::CONST;
		cv = make_unique<Bool>(v);
	}
	Const(ArgWrapper* p, BlockStmt* b): ctype(ConstType::FN) {
		etype = ExpType::CONST;
		cv = make_unique<Fn>(p, b);
	}
	unique_ptr<Object> code() override {
		return move(obj_clone(cv.get()));
	}
};

struct Idf : Expression {
	string name;

	Idf(const string& n): name(n) { etype = ExpType::ID; }
	unique_ptr<Object> code() override {
		if (env_stack->undefined(name))
			return make_unique<Error>(ErrorType::UNDEF, name);
		else {
			unique_ptr<Object> v = env_stack->get(name);
			unique_ptr<Object> v_ = move(obj_clone(v.get()));
			env_stack->insert(name, move(v));
			return move(v_);
		}
	}
};

struct PrefixExp : Expression {
	PrefixOp op;
	Expression* right;

	PrefixExp(PrefixOp o, Expression* r): op(o), right(r) { etype = ExpType::PREFIX; }
	~PrefixExp() { delete right; }
	unique_ptr<Object> code() override {
		unique_ptr<Object> rv = move(right->code());
		switch (op) {
			case PrefixOp::NEG:
				if (rv->otype == ObjType::INT)
					return make_unique<Int>(- obj_vcast<int>(rv.get()));
				else if (rv->otype == ObjType::FLT)
					return make_unique<Double>(- obj_vcast<double>(rv.get()));
				else
					return make_unique<Error>(ErrorType::UNSOP, prefix_str[op] + rv->str());
				break;
			case PrefixOp::NOT:
				if (rv->otype != ObjType::BOOL)
					return make_unique<Error>(ErrorType::UNSOP, prefix_str[op] + rv->str());
				else
					return make_unique<Bool>(! obj_vcast<bool>(rv.get()));
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
	unique_ptr<Object> code() override {
		unique_ptr<Object> lv = move(left->code());
		unique_ptr<Object> rv = move(right->code());

		if (lv->otype != rv->otype)
			return make_unique<Error>(ErrorType::TYPE, lv->str() + infix_str[op] + rv->str());
		
		ObjType t = lv->otype;
		bool v = false;
		bool inv = false;

		switch (op) {
			case InfixOp::ADD:
				switch (t) {
					case ObjType::INT: return make_unique<Int>(obj_vcast<int>(lv.get()) + obj_vcast<int>(rv.get()));
					case ObjType::FLT: return make_unique<Double>(obj_vcast<double>(lv.get()) + obj_vcast<double>(rv.get()));
					case ObjType::STR: return make_unique<String>(obj_vcast<string>(lv.get()) + obj_vcast<string>(rv.get()));
					default: inv = true;
				}
				break;
			case InfixOp::SUB:
				switch (t) {
					case ObjType::INT: return make_unique<Int>(obj_vcast<int>(lv.get()) - obj_vcast<int>(rv.get()));
					case ObjType::FLT: return make_unique<Double>(obj_vcast<double>(lv.get()) - obj_vcast<double>(rv.get()));
					default: inv = true;
				}
				break;
			case InfixOp::MUL:
				switch (t) {
					case ObjType::INT: return make_unique<Int>(obj_vcast<int>(lv.get()) * obj_vcast<int>(rv.get()));
					case ObjType::FLT: return make_unique<Double>(obj_vcast<double>(lv.get()) * obj_vcast<double>(rv.get()));
					default: inv = true;
				}
				break;
			case InfixOp::DIV:
				switch (t) {
					case ObjType::INT: return make_unique<Int>(obj_vcast<int>(lv.get()) / obj_vcast<int>(rv.get()));
					case ObjType::FLT: return make_unique<Double>(obj_vcast<double>(lv.get()) / obj_vcast<double>(rv.get()));
					default: inv = true;
				}
				break;
			case InfixOp::MOD:
				switch (t) {
					case ObjType::INT: return make_unique<Int>(obj_vcast<int>(lv.get()) % obj_vcast<int>(rv.get()));
					default: inv = true;
				}
				break;
			case InfixOp::EQ:
				switch (t) {
					case ObjType::INT: v = obj_vcast<int>(lv.get()) == obj_vcast<int>(rv.get()); break;
					case ObjType::FLT: v = obj_vcast<double>(lv.get()) == obj_vcast<double>(rv.get()); break;
					case ObjType::STR: v = obj_vcast<string>(lv.get()) == obj_vcast<string>(rv.get()); break;
					case ObjType::BOOL: v = obj_vcast<bool>(lv.get()) == obj_vcast<bool>(rv.get()); break;
					case ObjType::NONE: v = true; break;
					default: inv = true;
				}
				if (!inv) return make_unique<Bool>(v);
				break;
			case InfixOp::NE:
				switch (t) {
					case ObjType::INT: v = obj_vcast<int>(lv.get()) != obj_vcast<int>(rv.get()); break;
					case ObjType::FLT: v = obj_vcast<double>(lv.get()) != obj_vcast<double>(rv.get()); break;
					case ObjType::STR: v = obj_vcast<string>(lv.get()) != obj_vcast<string>(rv.get()); break;
					case ObjType::BOOL: v = obj_vcast<bool>(lv.get()) != obj_vcast<bool>(rv.get()); break;
					case ObjType::NONE: v = false; break;
					default: inv = true;
				}
				if (!inv) return make_unique<Bool>(v);
				break;
			case InfixOp::LT:
				switch (t) {
					case ObjType::INT: v = obj_vcast<int>(lv.get()) < obj_vcast<int>(rv.get()); break;
					case ObjType::FLT: v = obj_vcast<double>(lv.get()) < obj_vcast<double>(rv.get()); break;
					default: inv = true;
				}
				if (!inv) return make_unique<Bool>(v);
				break;
			case InfixOp::LE:
				switch (t) {
					case ObjType::INT: v = obj_vcast<int>(lv.get()) <= obj_vcast<int>(rv.get()); break;
					case ObjType::FLT: v = obj_vcast<double>(lv.get()) <= obj_vcast<double>(rv.get()); break;
					default: inv = true;
				}
				if (!inv) return make_unique<Bool>(v);
				break;
			case InfixOp::GT:
				switch (t) {
					case ObjType::INT: v = obj_vcast<int>(lv.get()) > obj_vcast<int>(rv.get()); break;
					case ObjType::FLT: v = obj_vcast<double>(lv.get()) > obj_vcast<double>(rv.get()); break;
					default: inv = true;
				}
				if (!inv) return make_unique<Bool>(v);
				break;
			case InfixOp::GE:
				switch (t) {
					case ObjType::INT: v = obj_vcast<int>(lv.get()) >= obj_vcast<int>(rv.get()); break;
					case ObjType::FLT: v = obj_vcast<double>(lv.get()) >= obj_vcast<double>(rv.get()); break;
					default: inv = true;
				}
				if (!inv) return make_unique<Bool>(v);
				break;
			case InfixOp::AND:
				if (t == ObjType::BOOL) return make_unique<Bool>(obj_vcast<bool>(lv.get()) && obj_vcast<bool>(rv.get()));
				else inv = true;
				break;
			case InfixOp::OR:
				if (t == ObjType::BOOL) return make_unique<Bool>(obj_vcast<bool>(lv.get()) || obj_vcast<bool>(rv.get()));
				else inv = true;
				break;
			default:
				cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { default }" << endl;
				exit(1);
		}

		if (inv) return make_unique<Error>(ErrorType::UNSOP, lv->str() + infix_str[op] + rv->str());
		return make_unique<Error>(ErrorType::UNK, "InfixExp::code()");
	}
};

#endif