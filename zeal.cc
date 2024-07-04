#include "zeal.hh"

EnvStack *env_stack;

void init() {
	env_stack = new EnvStack();
	env_stack->create("len", make_unique<Len>());
	env_stack->create("type", make_unique<Type>());
}

// --------------------------------

unordered_map<ObjType,string> objtype_str = {
	{ ObjType::INT, "obj::int" },
	{ ObjType::FLT, "obj::double" },
	{ ObjType::STR, "obj::string" },
	{ ObjType::BOOL, "obj::bool" },
	{ ObjType::NONE, "obj::null" },
	{ ObjType::ERR, "obj::error" },
	{ ObjType::FN, "obj::func" },
	{ ObjType::BF, "obj::builtin" },
	{ ObjType::LIST, "obj::list" },
	{ ObjType::DICT, "obj::dict" },
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

unique_ptr<Object> obj_clone(Object* obj) {
	switch (obj->otype) {
		case ObjType::INT: return move(make_unique<Int>(obj_vcast<int>(obj)));
		case ObjType::FLT: return move(make_unique<Double>(obj_vcast<double>(obj)));
		case ObjType::STR: return move(make_unique<String>(obj_vcast<string>(obj)));
		case ObjType::BOOL: return move(make_unique<Bool>(obj_vcast<bool>(obj)));
		case ObjType::NONE: return move(make_unique<Null>());
		case ObjType::ERR: return move(make_unique<Error>(((Error*)obj)->err_type, obj_vcast<string>(obj)));
		case ObjType::FN: return move(make_unique<Fn>(((Fn*)obj)->params, ((Fn*)obj)->body));
		case ObjType::BF: {
			switch (((BuiltIn*)obj)->btype) {
				case BfType::LEN: return move(make_unique<Len>());
				case BfType::TYPE: return move(make_unique<Type>());
				default: 
					cerr << "[error] Object* obj_clone(Object* obj) { ObjType::BF }";
					exit(1);
			}
		}
		default: 
			cerr << "[error] Object* obj_clone(Object* obj)";
			exit(1);
	}
}