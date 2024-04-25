#include "common-headers.hh"
#include "code.hh"

extern lpmode mode;
int temp_count=0;
map <string, int> symtab;

Expr_Attribute * process_ID(string * name)
{
	Expr_Attribute * ea;

	Code * code = new Code();

	if (mode == interpreter)
		ea = new Expr_Attribute(symtab[*name], NULL, NULL);
	else if (mode == compiler)
		ea = new Expr_Attribute(0, name, code, OT_ID);

	return ea;
}

Expr_Attribute * process_NUM(string * name)
{
	Expr_Attribute * ea;

	Code * code = new Code();

	if (mode == interpreter)
		ea = new Expr_Attribute(stoi(*name), NULL, NULL);
	else if (mode == compiler)
		ea = new Expr_Attribute(0, name, code, OT_NUM);
	return ea;
}

string operator_name(op_type op)
{
	string name; 
	switch (op)
	{
		case PLUS: 	name = " + "; break;
		case MINUS:	name = " - "; break;
		case MULT:	name = " * "; break;
		case DIV:	name = " / "; break;
		case UMINUS:	name = " - "; break;
		default:
				cerr << "Wrong operator type" << endl;
				exit(1);
				break;
	}
	return name;
}


Expr_Attribute * process_Expr(Expr_Attribute *left, op_type op, Expr_Attribute *right)
{
	Expr_Attribute * ea;
	int result;

	if (mode == interpreter)
	{
		switch (op)
		{
			case PLUS: 	result = left->get_value() + right->get_value(); break;
			case MINUS:	result = left->get_value() - right->get_value(); break;
			case MULT:	result = left->get_value() * right->get_value(); break;
			case DIV:	result = left->get_value() / right->get_value(); break;
			case UMINUS:
				if (right != NULL)
				{
					cerr << "Right operand must be NULL for Unary Minus" << endl;
					exit(1);
				}
				else	result =  - left->get_value();
				break;
			default:
				cerr << "Wrong operator type" << endl;
				exit(1);
				break;
		}
		ea = new Expr_Attribute(result, NULL, NULL);
	}
	else if (mode == compiler)
	{
		string * temp = new string ("t" + to_string(temp_count++));
		Statement * stmt;
		Code * code; 
		if (op == UMINUS)
		{
			stmt = new Statement(temp, op, left->get_place());

			code = left->get_code();
			code->append_statement(stmt);
		}
		else 
		{
			stmt = new Statement(temp, op, left->get_place(), right->get_place());
			code = left->get_code();
			code->append_list(right->get_code());
			code->append_statement(stmt);
		}

		operation_type z;
		switch (op)
		{
			case PLUS: 	z = OT_PLUS; break;
			case MINUS:	z = OT_MINUS; break;
			case MULT:	z = OT_MULT; break;
			case DIV:	z = OT_DIV; break;
			case UMINUS: z = OT_UMINUS; break;
			default:
				cerr << "Wrong operator type" << endl;
				exit(1);
				break;
		}
		ea = new Expr_Attribute(0, temp, code, z, left, right);
	}
	return ea;
}

Code * process_Asgn(string *lhs, Expr_Attribute *rhs)
{
	Expr_Attribute * ea;
	Statement * stmt;
	Code *code; 

	if (mode == interpreter)
	{
		if (lhs == NULL)
		   	cout << "> " << rhs->get_value() << endl;
		else
		{
			symtab[*lhs] = rhs->get_value();
			cout << "> " << *lhs << " = " << rhs->get_value() << endl;
		}
		code = new Code();
	}
	else if (mode == compiler)
	{
		if (lhs == NULL)
		{
			stmt = new Statement (rhs->get_place());
			code = rhs->get_code();
			code->append_statement(stmt);
			
			rhs->print_ast(1, true);
		}
		else
		{
			stmt = new Statement (lhs, COPY, rhs->get_place());
			code = rhs->get_code();
			code->append_statement(stmt);

			cout << "\n    Asgn:\n      LHS (Name : " << *lhs << ")\n      RHS ";
			rhs->print_ast(1, false);
		}
	}
	return code;
}

void process_finish(Code * code)
{ 	if (mode == compiler) 
	{
		cout << "\n\nThe three address code generated for the input is\n"; 
		code->print_code();
	}
}

Code * process_Stmt (Code * code)
{ 
	if (mode == compiler) 
		return code; 
	else return new Code();
}

Code * process_Stmt_List(Code *list, Code *code)
{ 
	if (mode == compiler) 
	{
		list->append_list(code);
		return  list;
	}
	else return new Code();
}

map<operation_type, string> op_print = {
	{OT_PLUS, "Plus"},
	{OT_MINUS, "Minus"},
	{OT_MULT, "Mult"},
	{OT_DIV, "Div"}
};

void Expr_Attribute::print_ast(int depth = 1, bool starter = false) {
	string space = "";
	for (int i = 0; i<depth+1; i++) space += "    ";

	if (lhs == NULL && rhs == NULL) {
		switch (t) {
			case OT_ID:
				if (starter) {
					cout << "\n    Name : " << *place;
				} else {
					cout << "(Name : " << *place << ")";
				}
				break;
			case OT_NUM:
				if (starter) {
					cout << "\n    Num : " << *place;
				} else {
					cout << "(Num : " << *place << ")";
				}
				break;
		}
		return;
	} else if (lhs != NULL && rhs == NULL) {
		if (!starter) cout << "(";
		cout << "\n" << space << "Arith: Uminus\n" << space << "  L_Opd ";
		lhs->print_ast(depth+1);
		if (!starter) cout << ")";
	} else {
		if (!starter) cout << "(";
		cout << "\n" << space << "Arith: " << op_print[t] << "\n" << space << "  L_Opd ";
		lhs->print_ast(depth+1);
		cout << "\n" << space << "  R_Opd ";
		rhs->print_ast(depth+1);
		if (!starter) cout << ")";
	}
}