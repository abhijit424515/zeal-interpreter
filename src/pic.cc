#include "pic.hh"

unordered_map<string,Object*> table;

// --------------------------------

unordered_map<ObjType,string> objtype_str = {
	{ ObjType::INT, "int" },
	{ ObjType::FLT, "float" },
	{ ObjType::STR, "string" },
	{ ObjType::BOOL, "bool" },
	{ ObjType::NONE, "None" },
	{ ObjType::ERR, "error" },
	{ ObjType::FUNC, "function" },
	{ ObjType::LIST, "list" },
	{ ObjType::DICT, "dict" },
};

unordered_map<PrefixOp,string> prefix_str = {
	{ PrefixOp::NEG, "-" },
	{ PrefixOp::NOT, "!" },
};

unordered_map<InfixOp,string> infix_str = {
	{ InfixOp::ADD, "+" },
	{ InfixOp::SUB, "-" },
	{ InfixOp::MUL, "*" },
	{ InfixOp::DIV, "/" },
	{ InfixOp::MOD, "%" },
	{ InfixOp::EQ, "==" },
	{ InfixOp::NE, "!=" },
	{ InfixOp::LT, "<" },
	{ InfixOp::LE, "<=" },
	{ InfixOp::GT, ">" },
	{ InfixOp::GE, ">=" },
	{ InfixOp::AND, "&&" },
	{ InfixOp::OR, "||" },
};

// --------------------------------

void repl_print(const string& s) {
	if (!repl) return;
	cout << ">> " << s << endl;
}