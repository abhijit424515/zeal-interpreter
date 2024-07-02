#ifndef SCOPE_HH
#define SCOPE_HH

#include "obj.hh"

struct Scope {
	unordered_map<string,Object*> scope;

	Object*& operator[](const string& name) {
		return scope[name];
	}
	Object*& find(const string& name) {
		if (scope.find(name) != scope.end()) return scope[name];
		cerr << "[error] Object* find(const string& name): " << name << " not found\n";
		exit(1);
	}
	void insert(const pair<string,Object*>& p) {
		scope.insert(p);
	}
	~Scope() {
		for (auto x: scope) delete x.second;
	}
};

struct EnvStack {
	vector<Scope*> stack;
	Scope* global_env;

	EnvStack() {
		global_env = new Scope();
		stack.push_back(global_env);
	}

	bool undefined(const string& name) {
		for (auto it = stack.rbegin(); it != stack.rend(); ++it)
			if ((*it)->scope.find(name) != (*it)->scope.end()) return false;
		return true;
	}
	bool redeclaration(const string& name) {
		if (stack.back()->scope.find(name) != stack.back()->scope.end()) return true;
		return false;
	}
	void create(const string& k, Object* v) {
		stack.back()->insert({k,v});
	}
	Object*& modify(const string& name) {
		for (auto it = stack.rbegin(); it != stack.rend(); ++it)
			if ((*it)->scope.find(name) != (*it)->scope.end()) 
				return (*it)->scope.find(name)->second;
				
		cerr << "[error] Object* find(const string& name): " << name << " not found\n";
		exit(1);
	}
	void pop() {
		if (stack.size() == 1) {
			cerr << "[error] void pop(): cannot pop global scope\n";
			exit(1);
		}
		delete stack.back();
		stack.pop_back();
	}

	~EnvStack() {
		for (auto& env : stack) delete env;
	}
};

#endif