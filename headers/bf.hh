#ifndef BUILTIN_HH
#define BUILTIN_HH

#include "node.hh"

// --------------------------------

struct Len: BuiltIn {
	Len(): BuiltIn("len", make_shared<ArgWrapper>(vector<string*>{ new string("x") })) { btype = BfType::LEN; }

	unique_ptr<Object> code(vector<unique_ptr<Object>> exps) const override {
		unique_ptr<Object> x = move(exps[0]);
		if (x->otype != ObjType::STR)
			return make_unique<Error>(ErrorType::TYPE, "bf::" + id + "() expects a string");

		return make_unique<Int>(obj_vcast<string>(x.get()).size());
	}
};

struct Type: BuiltIn {
	Type(): BuiltIn("type", make_shared<ArgWrapper>(vector<string*>{ new string("x") })) { btype = BfType::TYPE; }

	unique_ptr<Object> code(vector<unique_ptr<Object>> exps) const override {
		unique_ptr<Object> x = move(exps[0]);
		return make_unique<String>(objtype_str[x->otype], false);
	}
};

// --------------------------------



#endif