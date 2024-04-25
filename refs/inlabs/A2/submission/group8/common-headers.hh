# include <stdio.h>
# include <argp.h>
# include <iostream>
# include <string>
# include <sstream>
# include <map>

using namespace std;

class Code;

typedef enum {
	OT_PLUS,
	OT_MINUS,
	OT_MULT,
	OT_DIV,
	OT_UMINUS,
	OT_COPY,
	OT_NUM,
	OT_ID,
} operation_type;

class Expr_Attribute
{	
	int value;
	string * place;
	Code * code;
	
	Expr_Attribute *lhs;
	Expr_Attribute *rhs;
	operation_type t;

   public:
	Expr_Attribute() { }
	Expr_Attribute(int v, string *p, Code *c) { value=v; place=p; code=c;}
	Expr_Attribute(int v, string *p, Code *c, operation_type t, Expr_Attribute *lhs = NULL, Expr_Attribute *rhs = NULL) { value=v; place=p; code=c; this->t=t; this->lhs = lhs; this->rhs = rhs;}
	~Expr_Attribute();

	int get_value() 	{ return value; }
	string * get_place() 	{ return place; }
	Code * get_code()	{ return code; }

	void print_ast(int depth, bool starter);
};

typedef enum {
		compiler,
		interpreter
	} lpmode;

typedef enum {
	PLUS,
	MINUS,
	MULT,
	DIV,
	UMINUS,
	COPY
	} op_type;

Expr_Attribute * process_ID(string * name);
Expr_Attribute * process_NUM(string * name);
Expr_Attribute * process_Expr(Expr_Attribute *left, op_type op, Expr_Attribute *right);
Code * process_Asgn(string *lhs, Expr_Attribute *rhs);
void process_finish(Code * code);
Code * process_Stmt (Code * code);
Code * process_Stmt_List(Code *list, Code *stmt);
string operator_name(op_type op);


#if 0
// Code to test the class. Need to rename the file to .cc to compile 
// into an executable.
//
int main()
{

	string * p = new string ("t0");
	string * c = new string ("This is my code");

	Expr_Attribute * ea = new Expr_Attribute(10, p, c);

	Expr_Attribute * eb = ea;

	cout << "Value is " << eb->get_value() << endl;
	cout << "Place is " << *eb->get_place() << endl;
	cout << "Code is " << *eb->get_code() << endl;

	return 0;
}

#endif
