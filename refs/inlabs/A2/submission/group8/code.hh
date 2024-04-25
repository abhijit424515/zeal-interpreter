# include <stdio.h>
# include <assert.h>
# include <argp.h>
# include <iostream>
# include <string>
# include <list>
# include <sstream>
# include <map>

using namespace std;

class Statement 
{	
	string * lhs;
	op_type  op;
	string * opd1;
	string * opd2;

   public:
	Statement() { }
	Statement(string *l, op_type o, string *o1, string *o2) { lhs=l; op=o; opd1=o1; opd2=o2;}
	Statement(string *l, op_type o, string *o1) { lhs=l; op=o; opd1=o1; opd2=NULL;}
	Statement(string *l, string *o1) { lhs=l; op=COPY; opd1=o1; opd2=NULL;}
	Statement(string *o1) { lhs=NULL; op=COPY; opd1=o1; opd2=NULL;}
	~Statement();

	void print_stmt();
};

class Code
{
	list <Statement *> * stmt_list;

  public:
	Code() { stmt_list = new list <Statement *>; }
	~Code();

	void append_statement(Statement *s) { stmt_list->push_back(s); }
	void append_list (Code *c);
	list <Statement *> * get_list()	{ return stmt_list; }

	void print_code(); 
};

