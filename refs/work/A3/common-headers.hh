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
	UMINUS_O,
	AND_O,
	OR_O,
	NOT_O,
	LT_O,
	LE_O,
	GT_O,
	GE_O,
	NE_O,
	EQ_O
} op_type;

typedef enum {
	ARITHMETIC_E,
	BOOLEAN_E,
	RELATIONAL_E,
} expression_construct;

typedef enum {
	ASSIGN_S,
	PRINT_S,
	READ_S
} statement_type;

typedef enum {
	NARY_TAC,
	LABEL_TAC,
	GOTO_TAC,
	IFGOTO_TAC,
	WRITE_TAC,
	READ_TAC
} tac_type;

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

	TAC_Statement(string *lhs, op_type op = PLUS_O, string *o1 = NULL, string *o2 = NULL, tac_type t = NARY_TAC): lhs(lhs), op(op), o1(o1), o2(o2), t(t) {}
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

	Statement(string *s = NULL, Expression *e = NULL, statement_type t = ASSIGN_S, var_type v = INTEGER_V): s(s), e(e), t(t), v(v) {}
	void print_stmt_ast();
	TAC* gen_tac();
};

struct StatementList {
	list<Statement*> l;
};

struct Function {
	var_type return_type;
	string *name;
	list<tuple<var_type,string*>*>* params;
	bool is_defin;
	map<string,var_type> *local_scope;
	StatementList *stmts;

	Function(var_type return_type, string *name, list<tuple<var_type,string*>*>* params = NULL, bool is_defin = false, map<string,var_type> *local_scope = NULL, StatementList *stmts = NULL): return_type(return_type), name(name), params(params), is_defin(is_defin), local_scope(local_scope), stmts(stmts) {}
	void print_func_ast();
	void print_func_tac();
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
Expression* process_boolean_exp(Expression *lhs, op_type op, Expression *rhs);
Expression* process_ternary_exp(Expression *ternary, Expression *lhs, Expression *rhs);

Statement* process_assign(string *lhs, Expression *rhs);
Statement* process_print(Expression *exp);
Statement* process_read(string* name);

void print_program_ast();
void print_program_tac();