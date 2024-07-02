#ifndef OBJ_HH
#define OBJ_HH

#include "base.hh"

enum class ObjType {
	INT,
	FLT,
	STR,
	BOOL,
	NONE,
	ERR,
	FN,
	LIST,
	DICT,
};

struct Object {
	void* value;
	ObjType otype;

	virtual string str() const = 0;
};

// --------------------------------

struct Int : Object {
	Int(int v = 0) {
		otype = ObjType::INT;
		value = (void*)(new int(v));
	}

	string str() const override {
		return to_string(*(int*)value);
	}

	~Int() { delete (int*)value; }
};

struct Double : Object {
	Double(double v = 0) {
		otype = ObjType::FLT;
		value = (void*)(new double(v));
	}

	string str() const override {
		return to_string(*(double*)value);
	}

	~Double() { delete (double*)value; }
};

struct String : Object {
	String(const string& v = "") {
		otype = ObjType::STR;
		value = (void*)(new string(v));
	}

	string str() const override {
		return *(string*)value;
	}

	~String() { delete (string*)value; }
};

struct Bool : Object {
	Bool(bool v = false) {
		otype = ObjType::BOOL;
		value = (void*)(new bool(v));
	}

	string str() const override {
		return *(bool*)value ? "true" : "false";
	}

	~Bool() { delete (bool*)value; }
};

struct Null : Object {
	Null() {
		otype = ObjType::NONE;
		value = nullptr;
	}

	string str() const override {
		return "null";
	}

	~Null() {}
};

enum class ErrorType {
	UNDEF,
	REDECL,
	TYPE,
	UNSOP,
	ARG,
};

struct Error : Object {
	ErrorType err_type;

	Error(ErrorType et, const string& v): err_type(et) {
		otype = ObjType::ERR;
		value = (void*)(new string(v));
	}

	string str() const override {
		string v = *(string*)value;
		string s = "[error]";
		switch (err_type) {
			case ErrorType::UNDEF: s += "[undefined]: "; break;
			case ErrorType::REDECL: s += "[redeclaration]: "; break;
			case ErrorType::TYPE: s += "[type]: "; break;
			case ErrorType::UNSOP: s += "[unsupported_operation]: "; break;
		}
		return s + v;
	}

	~Error() { delete (string*)value; }
};

struct Fn;
Object* dup(Object* obj);

#endif