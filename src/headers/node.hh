#ifndef NODE_HH
#define NODE_HH

#include "obj.hh"

enum class PrefixOp;
enum class InfixOp;

extern bool repl;
extern unordered_map<string,Object*> table;
extern unordered_map<ObjType,string> objtype_str;
extern unordered_map<PrefixOp,string> prefix_str;
extern unordered_map<InfixOp,string> infix_str;

enum class NodeType {
	PROG,
	STMT,
	EXPR
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
};

enum class ConstType {
	INT,
	FLT,
	STR,
	BOOL,
	NONE,
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
		for (auto stmt : stmts) {
			delete stmt;
		}
	}
};

struct Program: Node {
	vector<Statement*> stmt_list;

	Program() { ntype = NodeType::PROG; }
	Program(StmtWrapper *s): stmt_list(s->stmts) { ntype = NodeType::PROG; }
	~Program() {
		for (auto stmt : stmt_list)
			delete stmt;
	}
	void code() override {
		for (auto stmt : stmt_list) stmt->code();
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
		if (table.find(id) != table.end()) {
			cerr << "[error] LetStmt::code()" << endl;
			exit(1);
		}
		table[id] = value->value;
	}
};

struct AsgStmt: Statement {
	string id;
	Expression* value;

	AsgStmt(const string& n, Expression* e): id(n), value(e) { stype = StmtType::ASG; }
	~AsgStmt() { delete value; }
	void code() override {
		value->code();
		if (table.find(id) == table.end()) {
			cerr << "[error] AsgStmt::code()" << endl;
			exit(1);
		}
		table[id] = value->value;
	}
};

struct RetStmt: Statement {
	Expression* value;

	RetStmt(Expression* e): value(e) { stype = StmtType::RETURN; }
	~RetStmt() { delete value; }
	void code() override {
		value->code();
	}
};

struct IfStmt: Statement {
	Expression* cond;
	Statement* then;
	Statement* els;

	IfStmt(Expression* c, Statement* t, Statement* e): cond(c), then(t), els(e) { stype = StmtType::IF; }
	~IfStmt() {
		delete cond;
		delete then;
		delete els;
	}
	void code() override {
		cond->code();
		if (cond->value->type != ObjType::BOOL) {
			cerr << "[error] IfStmt::code()" << endl;
			exit(1);
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
		value = new None();
	}
	Const(bool v): ctype(ConstType::BOOL) {
		etype = ExpType::CONST;
		value = new Bool(v);
	}
	~Const() {
		switch (ctype) {
			case ConstType::INT: delete (Int*)value; break;
			case ConstType::FLT: delete (Double*)value; break;
			case ConstType::STR: delete (String*)value; break;
			case ConstType::BOOL: delete (Bool*)value; break;
			case ConstType::NONE: delete (None*)value; break;
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
		if (table.find(name) == table.end()) value = new Error(UNDEF_ERR, name);
		else value = table[name];
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
				if (right->value->type == ObjType::INT)
					value = new Int(-(*(int*)(right->value->value)));
				else if (right->value->type == ObjType::FLT)
					value = new Double(-*(double*)(right->value->value));
				else
					value = new Error(UNSOP_ERR, prefix_str[op] + right->value->str());
				break;
			case PrefixOp::NOT:
				if (right->value->type != ObjType::BOOL)
					value = new Error(UNSOP_ERR, prefix_str[op] + right->value->str());
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
	~InfixExp() {
		delete left, right;
	}
	void code() override {
		left->code();
		right->code();
		void *lv = left->value->value, *rv = right->value->value;
		if (left->value->type == right->value->type) {
			ObjType t = left->value->type;
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
			if (inv) value = new Error(UNSOP_ERR, left->value->str() + infix_str[op] + right->value->str());
		} else value = new Error(TYPE_ERR, objtype_str[left->value->type] + infix_str[op] + objtype_str[right->value->type]);
	}
};

#endif