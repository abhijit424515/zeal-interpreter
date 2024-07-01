#include "pic.hh"

unordered_map<string,Object*> table;

// --------------------------------

void repl_print(const string& s) {
	if (!repl) return;
	cout << ">> " << s << endl;
}

// --------------------------------

string to_string(const PrefixOp& op) {
	string s;
	switch (op) {
		case NEG_PROP: s = "-"; break;
		case NOT_PROP: s = "!"; break;
		default: 
			cerr << "Invalid prefix operator" << endl;
			exit(1);
	}
	return s;
}

string to_string(const InfixOp& op) {
	string s;
	switch (op) {
		case ADD_INOP: s = "+"; break;
		case SUB_INOP: s = "-"; break;
		case MUL_INOP: s = "*"; break;
		case DIV_INOP: s = "/"; break;
		case MOD_INOP: s = "%"; break;
		case EQ_INOP: s = "=="; break;
		case NE_INOP: s = "!="; break;
		case LT_INOP: s = "<"; break;
		case LE_INOP: s = "<="; break;
		case GT_INOP: s = ">"; break;
		case GE_INOP: s = ">="; break;
		case AND_INOP: s = "&&"; break;
		case OR_INOP: s = "||"; break;
		default: 
			cerr << "Invalid infix operator" << endl;
			exit(1);
	}
	return s;
}