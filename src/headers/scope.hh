#ifndef SCOPE_HH
#define SCOPE_HH

#include "obj.hh"

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
	std::unique_ptr<Object> get(const std::string& name) {
		for (auto it = stack.rbegin(); it != stack.rend(); ++it)
			if ((*it)->scope.find(name) != (*it)->scope.end()) 
				return std::move((*it)->scope.find(name)->second);
				
		std::cerr << "[error] std::unique_ptr<Object> get(const std::string& name): " << name << " not found\n";
		exit(1);
	}
	void insert(const std::string& k, std::unique_ptr<Object> v) {
        stack.back()->insert(k, std::move(v));
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