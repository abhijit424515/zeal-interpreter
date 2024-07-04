#ifndef SCOPE_HH
#define SCOPE_HH

#include "obj.hh"

extern unordered_map<ObjType,string> objtype_str;

struct Scope {
	unordered_map<string,unique_ptr<Object>> scope;

	unique_ptr<Object>& operator[](const string& name) {
		return scope[name];
	}
	std::unique_ptr<Object>& find(const std::string& name) {
        auto it = scope.find(name);
        if (it != scope.end()) {
            return it->second;
        }
        std::cerr << "[error] std::unique_ptr<Object>& find(const std::string& name): " << name << " not found\n";
        exit(1);
    }
	void insert(const std::string& name, std::unique_ptr<Object> obj) {
        scope[name] = std::move(obj);
    }
};

struct EnvStack {
	std::vector<std::unique_ptr<Scope>> stack;

	EnvStack() { stack.push_back(std::make_unique<Scope>()); }

	bool undefined(const string& name) {
		for (auto it = stack.rbegin(); it != stack.rend(); ++it)
			if ((*it)->scope.find(name) != (*it)->scope.end()) return false;
		return true;
	}
	bool redeclaration(const string& name) {
		if (stack.back()->scope.find(name) != stack.back()->scope.end()) return true;
		return false;
	}
	unique_ptr<Object> find_and_own(const string& name) {
		for (auto it = stack.rbegin(); it != stack.rend(); ++it)
			if ((*it)->scope.find(name) != (*it)->scope.end()) 
				return move((*it)->scope.find(name)->second);
				
		cerr << "[error] unique_ptr<Object> get(const string& name): " << name << " not found\n";
		exit(1);
	}
	unique_ptr<Object> find_and_clone(const string& name) {
		for (auto it = stack.rbegin(); it != stack.rend(); ++it)
			if ((*it)->scope.find(name) != (*it)->scope.end()) {
				return move(obj_clone((*it)->scope.find(name)->second.get()));
			}
				
		cerr << "[error] unique_ptr<Object> get(const string& name): " << name << " not found\n";
		exit(1);
	}
	void create(const string& k, unique_ptr<Object> v) {
        stack.back()->insert(k, move(v));
    }
	void update(const string& k, unique_ptr<Object> v) {
		for (auto it = stack.rbegin(); it != stack.rend(); ++it)
			if ((*it)->scope.find(k) != (*it)->scope.end()) {
				(*it)->scope[k] = move(v);
				return;
			}
		cerr << "[error] void update(const string& k, unique_ptr<Object> v): " << k << " not found\n";
		exit(1);
	}
	void push_scope() {
		stack.push_back(std::make_unique<Scope>());
	}
	void pop_scope() {
		if (stack.size() == 1) {
			cerr << "[error] void pop(): cannot pop global scope\n";
			exit(1);
		}
		stack.pop_back();
	}
};

#endif