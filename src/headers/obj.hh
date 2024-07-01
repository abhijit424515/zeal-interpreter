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
	FUNC,
	LIST,
	DICT,
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
		type = ObjType::INT;
		value = (void*)(new int(v));
	}

	string str() const override {
		return to_string(*(int*)value);
	}

	~Int() { delete (int*)value; }
};

struct Double : Object {
	Double(double v = 0) {
		type = ObjType::FLT;
		value = (void*)(new double(v));
	}

	string str() const override {
		return to_string(*(double*)value);
	}

	~Double() { delete (double*)value; }
};

struct String : Object {
	String(const string& v = "") {
		type = ObjType::STR;
		value = (void*)(new string(v));
	}

	string str() const override {
		return *(string*)value;
	}

	~String() { delete (string*)value; }
};

struct Bool : Object {
	Bool(bool v = false) {
		type = ObjType::BOOL;
		value = (void*)(new bool(v));
	}

	string str() const override {
		return *(bool*)value ? "true" : "false";
	}

	~Bool() { delete (bool*)value; }
};

struct None : Object {
	None() {
		type = ObjType::NONE;
		value = nullptr;
	}

	string str() const override {
		return "None";
	}

	~None() {}
};

enum ErrorType {
	UNDEF_ERR,
	TYPE_ERR,
	UNSOP_ERR,
};

struct Error : Object {
	ErrorType err_type;

	Error(ErrorType et, const string& v): err_type(et) {
		type = ObjType::ERR;
		value = (void*)(new string(v));
	}

	string str() const override {
		string v = *(string*)value;
		string s = "[error]";
		switch (err_type) {
			case UNDEF_ERR: s += "[undefined]: "; break;
			case TYPE_ERR: s += "[type]: "; break;
			case UNSOP_ERR: s += "[unsupported_operation]: "; break;
		}
		return s + v;
	}

	~Error() { delete (string*)value; }
};

#endif