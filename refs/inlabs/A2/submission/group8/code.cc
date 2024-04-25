# include <stdio.h>
# include <assert.h>
# include <argp.h>
# include <iostream>
# include <string>
# include <list>
# include <sstream>
# include <map>

#include "common-headers.hh"
#include "code.hh"

using namespace std;

void Statement::print_stmt()
{
	if (op!=COPY)
	{
		if (opd2!=NULL)
			cout << *lhs << " = " << *opd1 << operator_name(op) << *opd2 << endl;
		else
		{
			assert(op == UMINUS);

			cout << *lhs << " = " << operator_name(op) << *opd1 << endl;
		}
	}
	else
	{
		if (lhs!=NULL)
			cout << *lhs << " = " << *opd1 << endl;
		else
			cout << *opd1 << endl;
	}
}

void Code::append_list (Code *c)	
{ 
	for (auto it = (c->get_list())->begin(); it != (c->get_list())->end(); ++it) 
	{
		if (*it != NULL)
			stmt_list->push_back(*it);
	}
}

void Code::print_code()
{
	for (auto it = stmt_list->begin(); it != stmt_list->end(); ++it) 
	{
		if (*it != NULL)
			(*it)->print_stmt();
	}
}
	
#if 0
// Code to test the class. 
//
lpmode mode = compiler;
int main()
{

	string * n1 = new string ("t1");
	string * n2 = new string ("t2");
	string * n3 = new string ("t3");
	string * n4 = new string ("t4");
	string * n5 = new string ("t5");
	string * n6 = new string ("t6");
	
	Statement *s1 = new Statement(n1, PLUS, n2, n3);
	Statement *s2 = new Statement(n4, MULT, n5, n6);
	Statement *s3 = new Statement(n4, n5 );
	Statement *s4 = new Statement(n6, DIV, n4 );
	Code *c1 = new Code();

	Code *c2 = new Code();

	c1->append_statement(s1);
	c1->append_statement(s3);
	c2->append_statement(s2);
	c2->append_statement(s4);

	c1->append_list(c2);

	c1->print_code();

	return 0;
}
#endif
