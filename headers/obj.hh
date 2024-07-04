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
	BF,
	LIST,
	DICT,
};

struct Object {
	void* value;
	ObjType otype;

	virtual string str() const = 0;
};

template<typename T>
static T obj_vcast(Object *v) { return *(T*)(void*)(v->value); }

unique_ptr<Object> obj_clone(Object* obj);

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
	bool quotes;

	String(const string& v = "", bool q = true): quotes(q) {
		otype = ObjType::STR;
		value = (void*)(new string(v));
	}

	string str() const override {
		string s = *(string*)value;
		return quotes ? '\"' + s + '\"' : s;
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
	UNK,
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
			case ErrorType::ARG: s += "[arg]: "; break;
			case ErrorType::UNK: s += "[unknown]: "; break;
		}
		return s + v;
	}

	~Error() { delete (string*)value; }
};

struct Fn;
Object* dup(Object* obj);

#endif