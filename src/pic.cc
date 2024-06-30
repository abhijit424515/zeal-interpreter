#include "pic.hh"

// --------------------------------

string to_string(const PrefixOp& op) {
	string s;
	switch (op) {
		case NEG_PROP: s = "-";
		case NOT_PROP: s = "!";
		default: 
			cerr << "Invalid prefix operator" << endl;
			exit(1);
	}
	return s;
}

string to_string(const InfixOp& op) {
	string s;
	switch (op) {
		case ADD_INOP: s = "+";
		case SUB_INOP: s = "-";
		case MUL_INOP: s = "*";
		case DIV_INOP: s = "/";
		case MOD_INOP: s = "%";
		case EQ_INOP: s = "==";
		case NE_INOP: s = "!=";
		case LT_INOP: s = "<";
		case LE_INOP: s = "<=";
		case GT_INOP: s = ">";
		case GE_INOP: s = ">=";
		case AND_INOP: s = "&&";
		case OR_INOP: s = "||";
		default: 
			cerr << "Invalid infix operator" << endl;
			exit(1);
	}
	return s;
}