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
	Object* value;
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

	LetStmt(const string& n, Expression* e): id(n), value(e) { 
		stype = LET_STMT;
		if (table.find(n) != table.end()) {
			cerr << "[error] LetStmt::LetStmt(const string& n, Expression* e)" << endl;
			exit(1);
		}
		table[id] = value->value;
	}
	~LetStmt() { delete value; }
	string str() const override {
		return value->str();
	}
};

struct RetStmt: Statement {
	Expression* value;

	RetStmt(Expression* e): value(e) { stype = RETURN_STMT; }
	~RetStmt() { delete value; }
	string str() const override {
		return value->str();
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

	ExpStmt(Expression* e): value(e) { 
		stype = EXP_STMT;
		repl_print(str());
	}
	~ExpStmt() { delete value; }
	string str() const override {
		return value->str();
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
	string str() const override {
		return value->str();
	}
};

struct Idf : Expression {
	string name;

	Idf(const string& n): name(n) { 
		etype = ID_EXP;
		if (table.find(n) == table.end()) {
			cerr << "[error] Idf::Idf(const string& n)" << endl;
			exit(1);
		}
		value = table[n];
	}
	string str() const override {
		return value->str();
	}
};

struct PrefixExp : Expression {
	PrefixOp op;
	Expression* right;

	PrefixExp(PrefixOp o, Expression* r): op(o), right(r) { 
		etype = PREFIX_EXP;
		switch (op) {
			case NEG_PROP:
				if (r->value->type == ObjType::INT_OBJ) {
					value = new Int(-(*(int*)(r->value->value)));
				} else if (r->value->type == ObjType::FLT_OBJ) {
					value = new Double(-*(double*)(r->value->value));
				} else {
					cerr << "[error] PrefixExp::PrefixExp(PrefixOp o, Expression* r) { NEG_PROP }" << endl;
					exit(1);
				}
				break;
			case NOT_PROP:
				if (r->value->type != ObjType::BOOL_OBJ) {
					cerr << "[error] PrefixExp::PrefixExp(PrefixOp o, Expression* r) { NOT_PROP }" << endl;
					exit(1);
				}
				value = new Bool(!*((bool*)r->value->value));
				break;
			default:
				cerr << "[error] PrefixExp::PrefixExp(PrefixOp o, Expression* r) { default }" << endl;
				exit(1);
		}
	}
	~PrefixExp() { delete right; }
	string str() const override {
		return value->str();
	}
};

struct InfixExp : Expression {
	Expression* left;
	InfixOp op;
	Expression* right;

	InfixExp(Expression* l, InfixOp o, Expression* r): left(l), op(o), right(r) { 
		etype = INFIX_EXP;
		void *lv = l->value->value, *rv = r->value->value;
		if (l->value->type == r->value->type) {
			ObjType t = l->value->type;
			bool v = false;
			switch (o) {
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

	~InfixExp() {
		delete left, right;
	}
	string str() const override {
		return value->str();
	}
};

#endif