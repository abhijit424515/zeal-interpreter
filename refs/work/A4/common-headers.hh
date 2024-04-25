# include <stdio.h>
# include <argp.h>
# include <iostream>
# include <iomanip>
# include <string>
# include <string.h>
# include <sstream>
# include <map>
# include <list>

using namespace std;

typedef enum {
    INTEGER_V,
    FLOAT_V,
    STRING_V,
    BOOL_V,
	VOID_V
} var_type;

typedef enum {
	PLUS_O,
	MINUS_O,
	MULT_O,
	DIV_O,
	AND_O,
	OR_O,
	LT_O,
	LE_O,
	GT_O,
	GE_O,
	NE_O,
	EQ_O,
	NOT_O,
	UMINUS_O,
} op_type;

typedef enum {
	ARITHMETIC_E,
	BOOLEAN_E,
	RELATIONAL_E,
} expression_construct;

typedef enum {
	ASSIGN_S,
	PRINT_S,
	READ_S,
	IF_S,
	IF_ELSE_S,
	DO_WHILE_S,
	WHILE_S,
	COMP_S
} statement_type;

typedef enum {
	NARY_TAC,
	LABEL_TAC,
	GOTO_TAC,
	IFGOTO_TAC,
	WRITE_TAC,
	READ_TAC
} tac_type;

typedef enum {
	SOURCE_T,
	TEMP_T,
	STEMP_T,
	CONSTANT_T
} tmp_type;

typedef enum {
	LOAD_R,
	ILOAD_R,
	NOT_R,
	AND_R,
	OR_R,
	BGTZ_R,
	STORE_R,
	GOTO_R,
	LABEL_R,
	MOVE_R,
	MOVT_R,
	MOVF_R,
	UMINUS_R,
	ADD_R,
	SUB_R,
	MUL_R,
	DIV_R,
	SLT_R,
	SLE_R,
	SGT_R,
	SGE_R,
	SNE_R,
	SEQ_R,
	READ_R,
	WRITE_R,
	LOADADDR_R,
} rtl_instr;

typedef enum {
    V0,
	V1,
    T0,
    T1,
    T2,
    T3,
    T4,
    T5,
    T6,
    T7,
    T8,
    T9,
    S0,
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
	F0,
    F2,
    F4,
    F6,
    F8,
    F10,
    F12,
    F14,
    F16,
    F18,
    F20,
    F22,
    F24,
    F26,
    F28,
    F30,
	A0,
	A1,
	A2,
	A3
} reg;

extern map<reg,string> print_reg;

struct Constant {
	string val;
	var_type t;

	Constant(string val, var_type t) {
		this->val = val;
		this->t = t;

		if (t == FLOAT_V) {
			const char *x = val.c_str();
			double y = strtod(x, NULL);
			ostringstream stream;
			stream << fixed << showpoint << setprecision(2) << y;
			this->val = stream.str();
		} else if (t == INTEGER_V) {
			const char *x = val.c_str();
			int y = atoi(x);
			ostringstream stream;
			stream << y;
			this->val = stream.str();
		}
	}
};

struct TAC_Statement {
	string *lhs;
	op_type op;
	string *o1;
	string *o2;
	tac_type t;
	
	tmp_type lhs_t;
	tmp_type o1_t;
	tmp_type o2_t;
	var_type lhs_v;
	var_type rhs_v;

	TAC_Statement(string *lhs, op_type op = PLUS_O, string *o1 = NULL, string *o2 = NULL, tac_type t = NARY_TAC, tmp_type lhs_t = SOURCE_T, tmp_type o1_t = SOURCE_T, tmp_type o2_t = SOURCE_T, var_type lhs_v = INTEGER_V, var_type rhs_v = INTEGER_V): lhs(lhs), op(op), o1(o1), o2(o2), t(t), lhs_t(lhs_t), o1_t(o1_t), o2_t(o2_t), lhs_v(lhs_v), rhs_v(rhs_v) {}
	void print_tac();
};

struct TAC {
	list<TAC_Statement*> l;
};

struct Expression {
    string *place;
	var_type t;
	op_type o;
    Expression *lhs, *rhs, *ternary;
	expression_construct e;
	bool is_const;
	TAC *code;

	Expression(string *place, var_type t, op_type o = PLUS_O, expression_construct e = ARITHMETIC_E, Expression *lhs = NULL, Expression *rhs = NULL, Expression *ternary = NULL, bool is_const = false): place(place), t(t), o(o), e(e), lhs(lhs), rhs(rhs), ternary(ternary), is_const(is_const) {}
	void print_exp_ast(int depth);
	TAC* gen_tac();
};

struct Statement {
	string *s;
	Expression *e;
	statement_type t;
	var_type v;
	TAC *code;
	Statement *s1,*s2;
	list<Statement*> *sl;

	Statement(string *s = NULL, Expression *e = NULL, statement_type t = ASSIGN_S, var_type v = INTEGER_V, Statement *s1 = NULL, Statement *s2 = NULL, list<Statement*> *sl = NULL): s(s), e(e), t(t), v(v), s1(s1), s2(s2), sl(sl) {}
	void print_stmt_ast(int depth);
	TAC* gen_tac();
};

struct StatementList {
	list<Statement*> l;
};

struct iarg {
	union {
		string *x;
		reg y;
	} z;
	int used;

	iarg(string *x) {
		z.x = x;
		used = 0;
	}
	iarg(reg y) {
		z.y = y;
		used = 1;
	}

	string to_string() {
		return (used == 0) ? *(z.x) : print_reg[z.y];
	}
};

struct RTL {
	rtl_instr i;
	iarg a1,a2,a3;
	bool is_int;

	RTL(rtl_instr i, bool is_int = true, iarg a1 = iarg(NULL), iarg a2 = iarg(NULL), iarg a3 = iarg(NULL)): i(i),is_int(is_int),a1(a1),a2(a2),a3(a3) {}
	void print_rtl();
};

struct Function {
	var_type return_type;
	string *name;
	list<tuple<var_type,string*>*>* params;
	bool is_defin;
	map<string,var_type> *local_scope;
	StatementList *stmts;
	TAC *code;
	list<RTL*> *rtl_list;

	Function(var_type return_type, string *name, list<tuple<var_type,string*>*>* params = NULL, bool is_defin = false, map<string,var_type> *local_scope = NULL, StatementList *stmts = NULL, TAC *code = NULL, list<RTL*> *rtl_list = NULL): return_type(return_type), name(name), params(params), is_defin(is_defin), local_scope(local_scope), stmts(stmts), code(code), rtl_list(rtl_list) {}
	
	void gen_tac();
	void gen_rtl();
	void print_func_ast();
	void print_func_tac();
	void print_func_rtl();
};

// --------------------------------

void fn_decl(tuple<var_type,string*>* header, list<tuple<var_type,string*>*>* param_list);
Function* fn_defn(tuple<var_type,string*> *header, list<tuple<var_type,string*>*> *param_list, map<string,var_type> *decl_map, StatementList *stmt_list);
tuple<var_type,string*>* fn_header(var_type v, string* name);
tuple<var_type,string*>* add_param(var_type v, string* name);
list<tuple<var_type,string*>*>* append_to_param_list(list<tuple<var_type,string*>*>* l, tuple<var_type,string*>* t);

StatementList* add_to_statement_list(StatementList* sl, Statement *s);
var_type check_undeclared_var(string id);
void check_redeclared_var(string id);
map<string,var_type>* save_and_get_stack_top();
void pop_scope();

string* create_ID(string *name);
Constant* create_CONST(string *name, var_type t);
Expression* process_ID(string *name);
Expression* process_CONST(Constant *cst);

list<string*>* add_to_decl_list(list<string*>* l, string *x);
void assign_type_to_decl_list(var_type t, list<string*>*l);

Expression* process_unary_exp(op_type op, Expression *lhs);
Expression* process_binary_exp(Expression *lhs, op_type op, Expression *rhs);
Expression* process_ternary_exp(Expression *ternary, Expression *lhs, Expression *rhs);

Statement* process_assign(string *lhs, Expression *rhs);
Statement* process_if_else(Expression *if_e, Statement *then_s, Statement *else_s);
Statement* process_if(Expression *if_e, Statement *then_s);
Statement* process_do_while(Statement *do_s, Expression *while_e);
Statement* process_while(Expression *while_e, Statement *do_s);
Statement* process_comp(StatementList *sl);
Statement* process_print(Expression *exp);
Statement* process_read(string* name);

void init();

void print_program_ast();
void print_program_tac();
void print_program_rtl();