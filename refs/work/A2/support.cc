# include <list>
# include <tuple>
# include <fcntl.h>
# include <assert.h>
# include <unistd.h>
# include "common-headers.hh"

extern bool show_ast;
extern bool demo_mode;
extern char* filename;

map<string,var_type> *global_map = new map<string,var_type>();
list<map<string,var_type>*> scope_stack = { global_map };

map<string,Function*> funcs;
list<string*> func_ordering;

// --------------------------------

map<var_type,string> print_var_type = {
    { VOID_V, "<void>" },
    { INTEGER_V, "<int>" },
    { FLOAT_V, "<float>" },
    { STRING_V, "<string>" },
    { BOOL_V, "<bool>" },
};

map<op_type, string> op_print = {
	{ PLUS_O, "Plus" },
	{ MINUS_O, "Minus" },
	{ MULT_O, "Mult" },
	{ DIV_O, "Div" },
    { UMINUS_O, "Uminus" },
    { AND_O, "AND" },
    { OR_O, "OR" },
    { NOT_O, "NOT" },
    { LT_O, "LT" },
	{ LE_O, "LE" },
	{ GT_O, "GT" },
	{ GE_O, "GE" },
	{ NE_O, "NE" },
	{ EQ_O, "EQ" },
};

// --------------------------------

void fn_decl(tuple<var_type,string*>* header, list<tuple<var_type,string*>*>* param_list) {
    string *name = get<1>(*header);
    if(*name != "main"){
        cerr << "File contains some feature of language level L5. The current implementation supports levels L1, L2, and L3.\n";
        exit(1);
    }
    Function *f = new Function(get<0>(*header), name, param_list);

    if (funcs.find(*name) != funcs.end()) {
        cerr << "FUNCTION REDECLARATION\n";
        exit(1);
    }
    funcs[*name] = f;
    pop_scope();
}

Function* fn_defn(tuple<var_type,string*> *header, list<tuple<var_type,string*>*> *param_list, map<string,var_type> *decl_map, StatementList *stmt_list) {
    string *name = get<1>(*header);
    Function *f;

    if (funcs.find(*name) == funcs.end()) {
        if(*name != "main"){
            cerr << "File: The main function should be defined\n";
            exit(1);
        } else if (get<0>(*header) != VOID_V) {
            cerr << "File: The return type of the main function should be void\n";
            exit(1);
        }
        f = new Function(get<0>(*header), name, param_list, true, decl_map, stmt_list);
        funcs[*name] = f;
    } else {
        if (get<0>(*header) != VOID_V) {
            cerr << "File: The return type of the main function should be void\n";
            exit(1);
        }
        f = funcs[*name];
        f->is_defin = true;

        if (get<0>(*header) != f->return_type) {
            cerr << "            Description: A declaration with a different return type exists for procedure " << *name << "_\n";
            exit(1);
        }
        if (!(
                (param_list == NULL && f->params == NULL) || (param_list != NULL && f->params != NULL)
            )) {
            cerr << "            Description: definition and declaration should have same number of params\n";
            exit(1);
        }
        if (param_list != NULL) {
            if (param_list->size() != f->params->size()) {
                cerr << "            Description: definition and declaration should have same number of params\n";
                exit(1);
            }
            auto j = f->params->begin();
            for (auto i = param_list->begin(); i != param_list->end(); i++) {
                if (get<0>(**i) != get<0>(**j)) {
                    cerr << "            Description: Types of parameters in declaration and definition do not match\n";
                    exit(1);
                }
                j++;
            }
        }

        f->params = param_list;
        f->local_scope = decl_map;
        f->stmts = stmt_list;
    }
    func_ordering.push_back(name);
    pop_scope();
    return f;
}

tuple<var_type,string*>* fn_header(var_type v, string* name) {
    scope_stack.push_back(new map<string,var_type>()); // created new scope
    tuple<var_type,string*> *t = new tuple<var_type,string*>();
    *t = make_tuple(v,name);
    return t;
}

tuple<var_type,string*>* add_param(var_type v, string* name) {
    check_redeclared_var(*name);
    (*(scope_stack.back()))[*name] = v;
    tuple<var_type,string*> *t = new tuple<var_type,string*>();
    *t = make_tuple(v,name);
    return t;
}

list<tuple<var_type,string*>*>* append_to_param_list(list<tuple<var_type,string*>*>* l, tuple<var_type,string*>* t) {
    if (l == NULL) l = new list<tuple<var_type,string*>*>();
    l->push_back(t);
    return l;
}

StatementList* add_to_statement_list(StatementList* sl, Statement *s) {
    if (sl == NULL) sl = new StatementList();
    if (s != NULL) sl->l.push_back(s);
    return sl;
}

var_type check_undeclared_var(string id) {
    for (auto it = scope_stack.rbegin(); it != scope_stack.rend(); it++) {
        if ((*it)->find(id) != (*it)->end()) {
            return (**it)[id];
        }
    }

    cerr << "            Description: Variable " << id << "_ has not been declared\n";
    exit(1);
}

void check_redeclared_var(string id) {
    if(scope_stack.back()->find(id) != scope_stack.back()->end()){
        cerr << "            Description: Variable is declared twice in the same scope\n";
        exit(1);
    }
}

map<string,var_type>* save_and_get_stack_top() {
    map<string,var_type>* top_scope = new map<string,var_type>();
    *top_scope = *(scope_stack.back());
    return top_scope;
}

void pop_scope() {
    assert(scope_stack.size() > 1);
    delete scope_stack.back();
    scope_stack.pop_back();
}

// ----------------------------------------------------------------

string* create_ID(string *name) {
    // [ ] TODO: need to fix for general case
    if (*name == "main") {
        cerr << "            Description: Variable main coincides with a procedure name\n";
        exit(1);
    }

    check_redeclared_var(*name);
    (*(scope_stack.back()))[*name] = INTEGER_V; // setting default var_type before assign_type_to_decl_list() changes it
    return name;
}

Constant* create_CONST(string *val, var_type t) {
    Constant *c = new Constant(*val,t);
    return c;
}

Expression* process_ID(string *name) {
    var_type t = check_undeclared_var(*name);
    Expression *e = new Expression(name, t);
    return e;
}

Expression* process_CONST(Constant *cst) {
    Expression *e = new Expression(&(cst->val),cst->t);
    e->is_const = true;
    return e;
}

list<string*>* add_to_decl_list(list<string*>* l, string *x) {
    if(l == NULL) l = new list<string*>();
    l->push_back(x);
    return l;
}

void assign_type_to_decl_list(var_type t, list<string*>*l) {
    if(t == VOID_V){
        cerr << "            Description: Variables cannot have a void type\n";
        exit(1);
    }
    for (auto it = l->begin(); it != l->end(); ++it)
		if (*it != NULL) 
            (*(scope_stack.back()))[*(*it)] = t;
}

Expression* process_unary_exp(op_type op, Expression *lhs) {
    // parsing error for other op_type values, also includes NULL case for lhs

    Expression *z;
    if (op == NOT_O) {
        if (lhs->t != BOOL_V) {
            cerr << "            Description: Wrong type for operand in Boolean expression\n";
            exit(1);
        } else
            z = new Expression(NULL,lhs->t,op,BOOLEAN_E,lhs);
    } else if (op == UMINUS_O) {
        if (!(lhs->t == FLOAT_V || lhs->t == INTEGER_V)) {
            cerr << "            Description: Wrong type of operand in UMINUS expression\n";
            exit(1);
        } else
            z = new Expression(NULL,lhs->t,op,ARITHMETIC_E,lhs);
    }
    return z;
}

Expression* process_binary_exp(Expression *lhs, op_type op, Expression *rhs) {
    // parsing error for UMINUS and NOT, also includes NULL cases for lhs and rhs

    map<op_type, string> error_print = {
        { PLUS_O, "PLUS" },
        { MINUS_O, "MINUS" },
        { MULT_O, "MULT" },
        { DIV_O, "DIV" }
    };

    if (lhs->t != rhs->t) {
        if (op == PLUS_O || op == MINUS_O || op == MULT_O || op == DIV_O)
            cerr << "            Description: Wrong type of operand in " << error_print[op] << " expression\n";
        else if (op == LT_O || op == LE_O || op == GT_O || op == GE_O || op == NE_O || op == EQ_O)
            cerr << "            Description: Wrong type of operand in Relational expression\n";
        else if (op == AND_O || op == OR_O)
            cerr << "            Description: Wrong type of operand in Boolean expression\n";
        exit(1);
    } else {
        if (op == PLUS_O || op == MINUS_O || op == MULT_O || op == DIV_O) {
            if (!(lhs->t == INTEGER_V || lhs->t == FLOAT_V)) {
                cerr << "            Description: Wrong type of operand in " << error_print[op] << " expression\n";
                exit(1);
            }
        }
        else if (op == LT_O || op == LE_O || op == GT_O || op == GE_O || op == NE_O || op == EQ_O) {
            if (!(lhs->t == INTEGER_V || lhs->t == FLOAT_V)) {
                cerr << "            Description: Wrong type of operand in Relational expression\n";
                exit(1);
            }
        } else if (op == AND_O || op == OR_O) {
            if (!(lhs->t == BOOL_V)) {
                cerr << "            Description: Wrong type of operand in Boolean expression\n";
                exit(1);
            }
        }
    }

    Expression *z;
    if (op == PLUS_O || op == MINUS_O || op == MULT_O || op == DIV_O)
        z = new Expression(NULL, lhs->t, op, ARITHMETIC_E, lhs, rhs);
    else if (op == LT_O || op == LE_O|| op == GT_O || op == GE_O || op == NE_O || op == EQ_O)
        z = new Expression(NULL, BOOL_V, op, RELATIONAL_E, lhs, rhs);
    else if (op == AND_O || op == OR_O)
        z = new Expression(NULL, BOOL_V, op, BOOLEAN_E, lhs, rhs);
    return z;
}

Expression* process_ternary_exp(Expression *ternary, Expression *lhs, Expression *rhs) {
    // parsing error for NULL cases for lhs, rhs, and ternary

    if (lhs->t != rhs->t) {
        cerr << "            Description: Different data types of the two operands of conditional ast\n";
        exit(1);
    }
    if (ternary->t != BOOL_V) {
        cerr << "            Description: Wrong type of condition in conditional ast\n";
        exit(1);
    }
    
    Expression *z = new Expression(NULL,lhs->t,PLUS_O,ARITHMETIC_E,lhs,rhs,ternary); // PLUS and ARITHMETIC_E are just dummy
    return z;
}

Statement* process_assign(string *name, Expression *rhs) {
    var_type t = check_undeclared_var(*name);
    if (t != rhs->t) {
        cerr << "            Description: Assignment statement data type not compatible\n";
        exit(1);
    }

    Statement *s = new Statement(name, rhs, ASSIGN_S);
    return s;
}

Statement* process_print(Expression *exp) {
    Statement *s = new Statement(NULL, exp, PRINT_S);
    return s;
}

Statement* process_read(string *name) {
    var_type v = check_undeclared_var(*name);

    if (!(v == INTEGER_V || v == FLOAT_V)) {
        cerr << "            Description: Only Int and Float variables are allowed in a Read Stmt\n";
        exit(1);
    }

    Statement *s = new Statement(name, NULL, READ_S, v);
    return s;
}

// --------------------------------

void print_program_ast() {
    if (show_ast) {
		int fd;
		size_t len_x = strlen(filename);
		size_t len_y = len_x + strlen(".ast") + 1;
		char *new_filename = (char*) malloc(len_y*sizeof(char));
		strcpy(new_filename, filename);
		strcat(new_filename, ".ast");
		
		int stdout_orig; 
		if (!demo_mode) {
			if ((fd = open(new_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
				cerr << "Error creating .ast file\n";
				exit(1);
			}
			stdout_orig = dup(STDOUT_FILENO);
			if (dup2(fd, STDOUT_FILENO) == -1) {
				cerr << "Error redirecting output to .ast file\n";
                close(fd);
				exit(1);
			}
		}
		
		for (string *elem : func_ordering) {
            Function* f = funcs[*elem];
            f->print_func_ast();
        }

        cout << flush;
		if (!demo_mode) {
			dup2(stdout_orig, STDOUT_FILENO);
			close(fd);
		}
  	}
}

void Function::print_func_ast() {
	cout << "**PROCEDURE: " << *name << "\n";
    cout << "	Return Type: " << print_var_type[return_type] << "\n";
    cout << "	Formal Parameters:\n";
    if (params != NULL) {
        for (tuple<var_type,string*>* param : *params){
            cout << "		" << *get<1>(*param) << "_  Type:" << print_var_type[get<0>(*param)] << "\n";
        }
    }
    cout << "**BEGIN: Abstract Syntax Tree\n";
    for (auto stmt : stmts->l) {
        stmt->print_stmt_ast();
    }
    cout << "**END: Abstract Syntax Tree\n";
}

// DEPTH starts from 0 (for statements, so 1 for assignments), +1 depth == 2 spaces
void Expression::print_exp_ast(int depth = 1) {
    string space = "         ";
    for (int i=0; i<depth; i++) space += "  ";

	if (lhs == NULL && rhs == NULL) { // CASE: id, constant
        if (!is_const)
            cout << "Name : " << *place << "_" << print_var_type[t];
        else {
            if (t == STRING_V)
                cout << "String : " << *place << print_var_type[t];
            else if (t == INTEGER_V || t == FLOAT_V)
                cout << "Num : " << *place << print_var_type[t];
            else
                cout << "Name : " << *place << print_var_type[t];
        }
    } else if (lhs != NULL && rhs == NULL) { // CASE: unary
        if (e == BOOLEAN_E) {
            cout << "\n" << space << "Condition: NOT<bool>";
            cout << "\n" << space << "  L_Opd (";
            lhs->print_exp_ast(depth+2);
            cout << ")";
        } else if (e == ARITHMETIC_E) {
            cout << "\n" << space << "Arith: Uminus" << print_var_type[lhs->t];
            cout << "\n" << space << "  L_Opd (";
            lhs->print_exp_ast(depth+2);
            cout << ")";
        }
    } else if (ternary != NULL) { // CASE: ternary
        ternary->print_exp_ast(depth+2);
        cout << "\n" << space << "    True_Part (";
        lhs->print_exp_ast(depth+4);
        cout << ")\n" << space << "    False_Part (";
        rhs->print_exp_ast(depth+4);
        cout << ")";
    } else {
        if (e == ARITHMETIC_E)
            cout << "\n" << space << "Arith: " << op_print[o];
        else 
            cout << "\n" << space << "Condition: " << op_print[o]; 

        if (e == RELATIONAL_E) 
            cout << print_var_type[BOOL_V];
        else
            cout << print_var_type[lhs->t];
        
        cout << "\n" << space << "  L_Opd (";
        lhs->print_exp_ast(depth+2);
        cout << ")\n" << space << "  R_Opd (";
        rhs->print_exp_ast(depth+2);
        cout << ")";
    }
}

void Statement::print_stmt_ast() {
    switch (t) {
		case ASSIGN_S: 
			cout << "         Asgn:\n";
			cout << "           LHS (Name : " << *s << "_" << print_var_type[e->t] << ")\n";
			cout << "           RHS (";
			e->print_exp_ast(2);
			cout << ")\n";
			break;
		case PRINT_S: 
			cout << "         Write: ";
			e->print_exp_ast(1);
            cout << "\n";
			break;
		case READ_S: 
			cout << "         Read: Name : " << *s << "_" << print_var_type[v] << "\n";
			break;
	}
}