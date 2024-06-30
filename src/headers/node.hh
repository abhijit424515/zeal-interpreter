#ifndef NODE_HH
#define NODE_HH

#include "base.hh"

enum NodeType {
	PROG_NODE,
	STMT_NODE,
	EXPR_NODE
};

enum StmtType {
	LET_STMT,
	RETURN_STMT,
	IF_STMT,
	EXP_STMT,
};

enum ExpType {
	CONST_EXP,
	ID_EXP,
	PREFIX_EXP,
	INFIX_EXP,
};

enum ConstType {
	INT_CST,
	FLT_CST,
	STR_CST,
	BOOL_CST,
};

enum PrefixOp {
	NEG_PROP,
	NOT_PROP,
};
string to_string(const PrefixOp& op);

enum InfixOp {
	ADD_INOP,
	SUB_INOP,
	MUL_INOP,
	DIV_INOP,
	MOD_INOP,
	EQ_INOP,
	NE_INOP,
	LT_INOP,
	LE_INOP,
	GT_INOP,
	GE_INOP,
	AND_INOP,
	OR_INOP,
};
string to_string(const InfixOp& op);

struct Node {
	NodeType ntype;
	virtual string str() const = 0;
};

static inline ostream& operator<<(ostream &out, const Node& n) {
	return out << n.str();
}

// --------------------------------

struct Statement: Node {
	StmtType stype;
	virtual string str() const = 0;
};

struct Expression: Node {
	ExpType etype;
	virtual string str() const = 0;
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

	Program() { ntype = PROG_NODE; }
	Program(StmtWrapper *s): stmt_list(s->stmts) { ntype = PROG_NODE; }
	~Program() {
		for (auto stmt : stmt_list)
			delete stmt;
	}
	string str() const override {
		string res;
		for (auto stmt : stmt_list)
			res += stmt->str() + "\n";
		return res;
	}
};

// --------------------------------

struct LetStmt: Statement {
	string id;
	Expression* value;

	LetStmt(const string& n, Expression* e): id(n), value(e) { stype = LET_STMT; }
	~LetStmt() { delete value; }
	string str() const override {
		return "let " + id + " = " + value->str() + ";";
	}
};

struct RetStmt: Statement {
	Expression* value;

	RetStmt(Expression* e): value(e) { stype = RETURN_STMT; }
	~RetStmt() { delete value; }
	string str() const override {
		return "return " + value->str() + ";";
	}
};

struct IfStmt: Statement {
	Expression* cond;
	Statement* then;
	Statement* els;

	IfStmt(Expression* c, Statement* t, Statement* e): cond(c), then(t), els(e) { stype = IF_STMT; }
	~IfStmt() {
		delete cond;
		delete then;
		delete els;
	}
	string str() const override {
		string res = "if (" + cond->str() + ") {\n";
		res += then->str() + "\n}";
		if (els) res += " else {\n" + els->str() + "\n}";
		return res;
	}
};

struct ExpStmt: Statement {
	Expression* value;

	ExpStmt(Expression* e): value(e) { stype = EXP_STMT; }
	~ExpStmt() { delete value; }
	string str() const override {
		return value->str() + ";";
	}
};

// --------------------------------

struct Const: Expression {
	void *value;
	ConstType ctype;

	Const(string v, ConstType t): ctype(t) {
		switch (t) {
			case INT_CST: value = new int(stoi(v)); break;
			case FLT_CST: value = new float(stof(v)); break;
			case STR_CST: value = new string(v); break;
			case BOOL_CST: value = new bool(v == "true"); break;
			default: 
				cerr << "Invalid constant type" << endl;
				exit(1);
		}
		etype = CONST_EXP;
	}
	Const(bool v): ctype(BOOL_CST) {
		value = new int(v);
		etype = CONST_EXP;
	}
	~Const() {
		switch (ctype) {
			case INT_CST: delete (int*)value; break;
			case FLT_CST: delete (float*)value; break;
			case STR_CST: delete (string*)value; break;
			default: 
				cerr << "Invalid constant type" << endl;
				exit(1);
		}
	}
	string str() const override {
		switch (ctype) {
			case INT_CST: return to_string(*(int*)value);
			case FLT_CST: return to_string(*(float*)value);
			case STR_CST: return *(string*)value;
			default: 
				cerr << "Invalid constant type" << endl;
				exit(1);
		}
	}
};

struct Idf : Expression {
	string name;

	Idf(const string& n): name(n) { etype = ID_EXP; }
	string str() const override {
		return name;
	}
};

struct PrefixExp : Expression {
	PrefixOp op;
	Expression* right;

	PrefixExp(PrefixOp o, Expression* r): op(o), right(r) { etype = PREFIX_EXP; }
	~PrefixExp() { delete right; }
	string str() const override {
		return "(" + to_string(op) + right->str() + ")";
	}
};

struct InfixExp : Expression {
	Expression* left;
	InfixOp op;
	Expression* right;

	InfixExp(Expression* l, InfixOp o, Expression* r): left(l), op(o), right(r) { etype = INFIX_EXP; }
	~InfixExp() {
		delete left, right;
	}
	string str() const override {
		return "(" + left->str() + " " + to_string(op) + " " + right->str() + ")";
	}
};

#endif