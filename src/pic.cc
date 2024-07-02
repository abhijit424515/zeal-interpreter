#include "pic.hh"

EnvStack *env_stack = new EnvStack();

// --------------------------------

unordered_map<ObjType,string> objtype_str = {
	{ ObjType::INT, "int" },
	{ ObjType::FLT, "float" },
	{ ObjType::STR, "string" },
	{ ObjType::BOOL, "bool" },
	{ ObjType::NONE, "null" },
	{ ObjType::ERR, "error" },
	{ ObjType::FN, "function" },
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

Object* dup(Object* obj) {
	switch (obj->otype) {
		case ObjType::INT: return new Int(*(int*)obj->value);
		case ObjType::FLT: return new Double(*(double*)obj->value);
		case ObjType::STR: return new String(*(string*)obj->value);
		case ObjType::BOOL: return new Bool(*(bool*)obj->value);
		case ObjType::NONE: return new Null();
		case ObjType::ERR: return new Error(((Error*)obj)->err_type, *(string*)obj->value);
		case ObjType::FN: return new Fn(((Fn*)obj)->params, ((Fn*)obj)->body);
		default: 
			cerr << "[error] Object* dup(Object* obj)";
			exit(1);
	}
}