#include <map>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "obj.hh"
using namespace std;

enum Type {
	INT,
	DOUBLE,
	STRING,
	BOOL,
	NONE,
	FUNCTION,
	LIST,
	DICT,
};

struct Object {
	void* value;
	Type type;

	virtual string str() const = 0;
};

static inline ostream& operator<<(ostream &out, const Object& obj) {
	return out << obj.str();
}

// --------------------------------

struct Int : Object {
	Int(int v = 0) {
		type = Type::INT;
		value = (void*)(new int(v));
	}

	string str() const override {
		return to_string(*(int*)value);
	}

	~Int() { delete (int*)value; }
};

struct Double : Object {
	Double(double v = 0) {
		type = Type::DOUBLE;
		value = (void*)(new double(v));
	}

	string str() const override {
		return to_string(*(double*)value);
	}

	~Double() { delete (double*)value; }
};

struct String : Object {
	String(const string& v = "") {
		type = Type::STRING;
		value = (void*)(new string(v));
	}

	string str() const override {
		return *(string*)value;
	}

	~String() { delete (string*)value; }
};

struct Bool : Object {
	Bool(bool v = false) {
		type = Type::BOOL;
		value = (void*)(new bool(v));
	}

	string str() const override {
		return *(bool*)value ? "true" : "false";
	}

	~Bool() { delete (bool*)value; }
};

struct None : Object {
	None() {
		type = Type::NONE;
		value = nullptr;
	}

	string str() const override {
		return "None";
	}
};

struct Function : Object {
	Function(vector<Object*> param_list, vector<Statement*> stmt_list) {
		type = Type::FUNCTION;
		value = nullptr;
	}

	string str() const override {
		return "Function";
	}
};