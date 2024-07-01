#ifndef NODE_HH
#define NODE_HH

#include "obj.hh"

extern unordered_map<string,Object*> table;
extern bool repl;

enum NodeType {
	PROG_NODE,
	STMT_NODE,
	EXPR_NODE
};

enum StmtType {
	LET_STMT,
	ASG_STMT,
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
	NONE_CST,
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

	Program() { ntype = PROG_NODE; }
	Program(StmtWrapper *s): stmt_list(s->stmts) { ntype = PROG_NODE; }
	~Program() {
		for (auto stmt : stmt_list)
			delete stmt;
	}
	void code() override {
		for (auto stmt : stmt_list)
			stmt->code();
	}
};

// --------------------------------

struct LetStmt: Statement {
	string id;
	Expression* value;

	LetStmt(const string& n, Expression* e): id(n), value(e) { stype = LET_STMT; }
	~LetStmt() { delete value; }
	void code() override {
		value->code();
		cout << "[test] " << id << endl;
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

	AsgStmt(const string& n, Expression* e): id(n), value(e) { stype = ASG_STMT; }
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

	RetStmt(Expression* e): value(e) { stype = RETURN_STMT; }
	~RetStmt() { delete value; }
	void code() override {
		value->code();
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
	void code() override {
		if (cond->value->type != ObjType::BOOL_OBJ) {
			cerr << "[error] IfStmt::code()" << endl;
			exit(1);
		}
		cond->code();
		if (*(bool*)(cond->value->value)) then->code();
		else els->code();
	}
};

struct ExpStmt: Statement {
	Expression* value;

	ExpStmt(Expression* e): value(e) { 
		stype = EXP_STMT;
	}
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
		etype = CONST_EXP;
		switch (t) {
			case INT_CST: value = new Int(stoi(v)); break;
			case FLT_CST: value = new Double(stof(v)); break;
			case STR_CST: value = new String(v); break;
			case NONE_CST: value = new None(); break;
			default: 
				cerr << "[error] Const::Const(string v, ConstType t)" << endl;
				exit(1);
		}
	}
	Const(bool v): ctype(BOOL_CST) {
		etype = CONST_EXP;
		value = new Bool(v);
	}
	~Const() {
		switch (ctype) {
			case INT_CST: delete (Int*)value; break;
			case FLT_CST: delete (Double*)value; break;
			case STR_CST: delete (String*)value; break;
			case BOOL_CST: delete (Bool*)value; break;
			case NONE_CST: delete (None*)value; break;
			default: 
				cerr << "[error] Const::~Const()" << endl;
				exit(1);
		}
	}
	void code() override {}
};

struct Idf : Expression {
	string name;

	Idf(const string& n): name(n) { etype = ID_EXP; }
	void code() override {
		if (table.find(name) == table.end()) {
			cerr << "[error] Idf::Idf(const string& n)" << endl;
			exit(1);
		}
		value = table[name];
	}
};

struct PrefixExp : Expression {
	PrefixOp op;
	Expression* right;

	PrefixExp(PrefixOp o, Expression* r): op(o), right(r) { etype = PREFIX_EXP; }
	~PrefixExp() { delete right; }
	void code() override {
		switch (op) {
			case NEG_PROP:
				if (right->value->type == ObjType::INT_OBJ) {
					value = new Int(-(*(int*)(right->value->value)));
				} else if (right->value->type == ObjType::FLT_OBJ) {
					value = new Double(-*(double*)(right->value->value));
				} else {
					cerr << "[error] PrefixExp::PrefixExp(PrefixOp o, Expression* r) { NEG_PROP }" << endl;
					exit(1);
				}
				break;
			case NOT_PROP:
				if (right->value->type != ObjType::BOOL_OBJ) {
					cerr << "[error] PrefixExp::PrefixExp(PrefixOp o, Expression* r) { NOT_PROP }" << endl;
					exit(1);
				}
				value = new Bool(!*((bool*)right->value->value));
				break;
			default:
				cerr << "[error] PrefixExp::PrefixExp(PrefixOp o, Expression* r) { default }" << endl;
				exit(1);
		}
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
	void code() override {
		void *lv = left->value->value, *rv = right->value->value;
		if (left->value->type == right->value->type) {
			ObjType t = left->value->type;
			bool v = false;
			switch (op) {
				case ADD_INOP:
					if (t == ObjType::INT_OBJ) {
						value = new Int(*(int*)lv + *(int*)rv);
					} else if (t == ObjType::FLT_OBJ) {
						value = new Double(*(double*)lv + *(double*)rv);
					} else if (t == ObjType::STR_OBJ) {
						value = new String(*(string*)lv + *(string*)rv);
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { ADD_INOP }" << endl;
						exit(1);
					}
					break;
				case SUB_INOP:
					if (t == ObjType::INT_OBJ) {
						value = new Int(*(int*)lv - *(int*)rv);
					} else if (t == ObjType::FLT_OBJ) {
						value = new Double(*(double*)lv - *(double*)rv);
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { SUB_INOP }" << endl;
						exit(1);
					}
					break;
				case MUL_INOP:
					if (t == ObjType::INT_OBJ) {
						value = new Int(*(int*)lv * *(int*)rv);
					} else if (t == ObjType::FLT_OBJ) {
						value = new Double(*(double*)lv * *(double*)rv);
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { MUL_INOP }" << endl;
						exit(1);
					}
					break;
				case DIV_INOP:
					if (t == ObjType::INT_OBJ) {
						value = new Int(*(int*)lv / *(int*)rv);
					} else if (t == ObjType::FLT_OBJ) {
						value = new Double(*(double*)lv / *(double*)rv);
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { DIV_INOP }" << endl;
						exit(1);
					}
					break;
				case MOD_INOP:
					if (t == ObjType::INT_OBJ) {
						value = new Int(*(int*)lv % *(int*)rv);
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { MOD_INOP }" << endl;
						exit(1);
					}
					break;
				case EQ_INOP:
					if (t == ObjType::INT_OBJ) {
						v = *(int*)lv == *(int*)rv;
					} else if (t == ObjType::FLT_OBJ) {
						v = *(double*)lv == *(double*)rv;
					} else if (t == ObjType::STR_OBJ) {
						v = *(string*)lv == *(string*)rv;
					} else if (t == ObjType::BOOL_OBJ) {
						v = *(bool*)lv == *(bool*)rv;
					} else if (t == ObjType::NONE_OBJ) {
						v = true;
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { EQ_INOP }" << endl;
						exit(1);
					}
					value = new Bool(v);
					break;
				case NE_INOP:
					if (t == ObjType::INT_OBJ) {
						v = *(int*)lv != *(int*)rv;
					} else if (t == ObjType::FLT_OBJ) {
						v = *(double*)lv != *(double*)rv;
					} else if (t == ObjType::STR_OBJ) {
						v = *(string*)lv != *(string*)rv;
					} else if (t == ObjType::BOOL_OBJ) {
						v = *(bool*)lv != *(bool*)rv;
					} else if (t == ObjType::NONE_OBJ) {
						v = false;
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { NE_INOP }" << endl;
						exit(1);
					}
					value = new Bool(v);
					break;
				case LT_INOP:
					if (t == ObjType::INT_OBJ) {
						v = *(int*)lv < *(int*)rv;
					} else if (t == ObjType::FLT_OBJ) {
						v = *(double*)lv < *(double*)rv;
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { LT_INOP }" << endl;
						exit(1);
					}
					value = new Bool(v);
					break;
				case LE_INOP:
					if (t == ObjType::INT_OBJ) {
						v = *(int*)lv <= *(int*)rv;
					} else if (t == ObjType::FLT_OBJ) {
						v = *(double*)lv <= *(double*)rv;
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { LE_INOP }" << endl;
						exit(1);
					}
					value = new Bool(v);
					break;
				case GT_INOP:
					if (t == ObjType::INT_OBJ) {
						v = *(int*)lv > *(int*)rv;
					} else if (t == ObjType::FLT_OBJ) {
						v = *(double*)lv > *(double*)rv;
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { GT_INOP }" << endl;
						exit(1);
					}
					value = new Bool(v);
					break;
				case GE_INOP:
					if (t == ObjType::INT_OBJ) {
						v = *(int*)lv >= *(int*)rv;
					} else if (t == ObjType::FLT_OBJ) {
						v = *(double*)lv >= *(double*)rv;
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { GE_INOP }" << endl;
						exit(1);
					}
					value = new Bool(v);
					break;
				case AND_INOP:
					if (t == ObjType::BOOL_OBJ) {
						value = new Bool(*(bool*)lv && *(bool*)rv);
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { AND_INOP }" << endl;
						exit(1);
					}
					break;
				case OR_INOP:
					if (t == ObjType::BOOL_OBJ) {
						value = new Bool(*(bool*)lv || *(bool*)rv);
					} else {
						cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { OR_INOP }" << endl;
						exit(1);
					}
					break;
				default:
					cerr << "[error] InfixExp::InfixExp(Expression* l, InfixOp o, Expression* r) { default }" << endl;
					exit(1);
			}
		}
	}
};

#endif