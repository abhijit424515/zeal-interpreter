#ifndef OBJ_HH
#define OBJ_HH

#include "base.hh"

enum ObjType {
	INT_OBJ,
	FLT_OBJ,
	STR_OBJ,
	BOOL_OBJ,
	NONE_OBJ,
	FUNC_OBJ,
	LIST_OBJ,
	DICT_OBJ,
};

struct Object {
	void* value;
	ObjType type;

	virtual string str() const = 0;
};

static inline ostream& operator<<(ostream &out, const Object& obj) {
	return out << obj.str();
}

// --------------------------------

struct Int : Object {
	Int(int v = 0) {
		type = ObjType::INT_OBJ;
		value = (void*)(new int(v));
	}

	string str() const override {
		return to_string(*(int*)value);
	}

	~Int() { delete (int*)value; }
};

struct Double : Object {
	Double(double v = 0) {
		type = ObjType::FLT_OBJ;
		value = (void*)(new double(v));
	}

	string str() const override {
		return to_string(*(double*)value);
	}

	~Double() { delete (double*)value; }
};

struct String : Object {
	String(const string& v = "") {
		type = ObjType::STR_OBJ;
		value = (void*)(new string(v));
	}

	string str() const override {
		return *(string*)value;
	}

	~String() { delete (string*)value; }
};

struct Bool : Object {
	Bool(bool v = false) {
		type = ObjType::BOOL_OBJ;
		value = (void*)(new bool(v));
	}

	string str() const override {
		return *(bool*)value ? "true" : "false";
	}

	~Bool() { delete (bool*)value; }
};

struct None : Object {
	None() {
		type = ObjType::NONE_OBJ;
		value = nullptr;
	}

	string str() const override {
		return "None";
	}

	~None() {}
};

#endif