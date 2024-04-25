#include <list>
#include <vector>
#include <algorithm>
#include <tuple>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include "common-headers.hh"

extern bool show_ast;
extern bool show_tac;
extern bool show_rtl;
extern bool stop_after_ast;
extern bool stop_after_tac;
extern bool stop_after_rtl;
extern bool demo_mode;
extern char *filename;

map<string, var_type> *global_map = new map<string, var_type>();
list<map<string, var_type> *> scope_stack = {global_map};

map<string, Function *> funcs;
list<string *> func_ordering;
vector<tuple<string *, string *>> globals;

void add_global_string(string *x)
{
    for (auto z : globals)
        if (*(get<0>(z)) == *x)
            return;
    int n = globals.size();
    string *y = new string("_str_" + to_string(n));
    globals.push_back(make_tuple(x, y));
}

string *get_global_string(string *x)
{
    for (auto z : globals)
        if (*(get<0>(z)) == *x)
            return get<1>(z);
    return NULL;
}

map<reg, string> print_reg = {
    {V0, "v0"},
    {V1, "v1"},
    {T0, "t0"},
    {T1, "t1"},
    {T2, "t2"},
    {T3, "t3"},
    {T4, "t4"},
    {T5, "t5"},
    {T6, "t6"},
    {T7, "t7"},
    {T8, "t8"},
    {T9, "t9"},
    {S0, "s0"},
    {S1, "s1"},
    {S2, "s2"},
    {S3, "s3"},
    {S4, "s4"},
    {S5, "s5"},
    {S6, "s6"},
    {S7, "s7"},
    {F0, "f0"},
    {F2, "f2"},
    {F4, "f4"},
    {F6, "f6"},
    {F8, "f8"},
    {F10, "f10"},
    {F12, "f12"},
    {F14, "f14"},
    {F16, "f16"},
    {F18, "f18"},
    {F20, "f20"},
    {F22, "f22"},
    {F24, "f24"},
    {F26, "f26"},
    {F28, "f28"},
    {F30, "f30"},
    {A0, "a0"},
    {A1, "a1"},
    {A2, "a2"},
    {A3, "a3"},
};

list<tuple<reg, bool>> reglist_temps = {
    {V0, false},
    {T0, false},
    {T1, false},
    {T2, false},
    {T3, false},
    {T4, false},
    {T5, false},
    {T6, false},
    {T7, false},
    {T8, false},
    {T9, false},
    {S0, false},
    {S1, false},
    {S2, false},
    {S3, false},
    {S4, false},
    {S5, false},
    {S6, false},
    {S7, false},
};

list<tuple<reg, bool>> reglist_flts = {
    {F2, false},
    {F4, false},
    {F6, false},
    {F8, false},
    {F10, false},
    {F12, false},
    {F14, false},
    {F16, false},
    {F18, false},
    {F20, false},
    {F22, false},
    {F24, false},
    {F26, false},
    {F28, false},
    {F30, false},
};

list<tuple<reg, bool>> reglist_args = {
    {A0, false},
    {A1, false},
    {A2, false},
    {A3, false},
};

void reset_registers()
{
    for (auto &r : reglist_temps)
        get<1>(r) = false;
    for (auto &r : reglist_flts)
        get<1>(r) = false;
    for (auto &r : reglist_args)
        get<1>(r) = false;
}

bool is_reg_free(reg x)
{
    for (auto &r : reglist_temps)
    {
        if (get<0>(r) == x)
        {
            return !get<1>(r);
        }
    }
    for (auto &r : reglist_flts)
    {
        if (get<0>(r) == x)
        {
            return !get<1>(r);
        }
    }
    for (auto &r : reglist_args)
    {
        if (get<0>(r) == x)
        {
            return !get<1>(r);
        }
    }
    return false;
}

reg get_free_register(bool is_int)
{
    list<tuple<reg, bool>> *regl;
    if (is_int)
        regl = &reglist_temps;
    else
        regl = &reglist_flts;

    for (auto &r : *regl)
    {
        if (!get<1>(r))
        {
            get<1>(r) = true;
            return get<0>(r);
        }
    }
    cerr << "Insufficient number of registers\n";
    exit(1);
}

reg get_free_arg_register()
{
    for (auto &r : reglist_args)
    {
        if (!get<1>(r))
        {
            get<1>(r) = true;
            return get<0>(r);
        }
    }
    cerr << "Insufficient number of arg registers\n";
    exit(1);
}

void release_register(reg x)
{
    for (auto &r : reglist_temps)
    {
        if (get<0>(r) == x)
        {
            get<1>(r) = false;
            return;
        }
    }
    for (auto &r : reglist_flts)
    {
        if (get<0>(r) == x)
        {
            get<1>(r) = false;
            return;
        }
    }
    for (auto &r : reglist_args)
    {
        if (get<0>(r) == x)
        {
            get<1>(r) = false;
            return;
        }
    }
}

tmp_type find_tmp_type(string *x, bool is_const)
{
    if (is_const)
        return CONSTANT_T;
    if ((*x)[x->size() - 1] == '_')
        return SOURCE_T;
    if ((*x)[0] == 's')
        return STEMP_T;
    return TEMP_T;
}

// --------------------------------

int temp_count = 0;
string *get_new_temp()
{
    string *temp = new string("temp" + to_string(temp_count++));
    return temp;
}

int stemp_count = 0;
string *get_new_stemp()
{
    string *temp = new string("stemp" + to_string(stemp_count++));
    return temp;
}

int label_count = 0;
string *get_new_label()
{
    string *temp = new string("Label" + to_string(label_count++));
    return temp;
}

// --------------------------------

map<var_type, string> print_var_type = {
    {VOID_V, "<void>"},
    {INTEGER_V, "<int>"},
    {FLOAT_V, "<float>"},
    {STRING_V, "<string>"},
    {BOOL_V, "<bool>"},
};

map<op_type, string> op_print = {
    {PLUS_O, "Plus"},
    {MINUS_O, "Minus"},
    {MULT_O, "Mult"},
    {DIV_O, "Div"},
    {UMINUS_O, "Uminus"},
    {AND_O, "AND"},
    {OR_O, "OR"},
    {NOT_O, "NOT"},
    {LT_O, "LT"},
    {LE_O, "LE"},
    {GT_O, "GT"},
    {GE_O, "GE"},
    {NE_O, "NE"},
    {EQ_O, "EQ"},
};

map<op_type, string> print_op_type = {
    {PLUS_O, "+"},
    {MINUS_O, "-"},
    {MULT_O, "*"},
    {DIV_O, "/"},
    {UMINUS_O, "-"},
    {AND_O, "&&"},
    {OR_O, "||"},
    {NOT_O, "!"},
    {LT_O, "<"},
    {LE_O, "<="},
    {GT_O, ">"},
    {GE_O, ">="},
    {NE_O, "!="},
    {EQ_O, "=="},
};

// --------------------------------

void fn_decl(tuple<var_type, string *> *header, list<tuple<var_type, string *> *> *param_list)
{
    string *name = get<1>(*header);
    if (*name != "main")
    {
        cerr << "File contains some feature of language level L5. The current implementation supports levels L1, L2, and L3.\n";
        exit(1);
    }
    if (funcs.find(*name) != funcs.end())
    {
        cerr << "Procedure main is already defined once\n"; // main is hardcoded
        exit(1);
    }

    Function *f = new Function(get<0>(*header), name, param_list);
    funcs[*name] = f;
    pop_scope();
}

Function *fn_defn(tuple<var_type, string *> *header, list<tuple<var_type, string *> *> *param_list, map<string, var_type> *decl_map, StatementList *stmt_list)
{
    string *name = get<1>(*header);
    Function *f;

    if (funcs.find(*name) == funcs.end())
    {
        if (*name != "main")
        {
            cerr << "File: The main function should be defined\n";
            exit(1);
        }
        else if (get<0>(*header) != VOID_V)
        {
            cerr << "File: The return type of the main function should be void\n";
            exit(1);
        }
        f = new Function(get<0>(*header), name, param_list, true, decl_map, stmt_list);
        funcs[*name] = f;
    }
    else
    {
        if (get<0>(*header) != VOID_V)
        {
            cerr << "File: The return type of the main function should be void\n";
            exit(1);
        }
        f = funcs[*name];
        f->is_defin = true;

        if (get<0>(*header) != f->return_type)
        {
            cerr << "            Description: A declaration with a different return type exists for procedure " << *name << "_\n";
            exit(1);
        }
        if (!(
                (param_list == NULL && f->params == NULL) || (param_list != NULL && f->params != NULL)))
        {
            cerr << "            Description: definition and declaration should have same number of params\n";
            exit(1);
        }
        if (param_list != NULL)
        {
            if (param_list->size() != f->params->size())
            {
                cerr << "            Description: definition and declaration should have same number of params\n";
                exit(1);
            }
            auto j = f->params->begin();
            for (auto i = param_list->begin(); i != param_list->end(); i++)
            {
                if (get<0>(**i) != get<0>(**j))
                {
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

tuple<var_type, string *> *fn_header(var_type v, string *name)
{
    scope_stack.push_back(new map<string, var_type>()); // created new scope
    tuple<var_type, string *> *t = new tuple<var_type, string *>();
    *t = make_tuple(v, name);
    return t;
}

tuple<var_type, string *> *add_param(var_type v, string *name)
{
    check_redeclared_var(*name);
    (*(scope_stack.back()))[*name] = v;
    tuple<var_type, string *> *t = new tuple<var_type, string *>();
    *t = make_tuple(v, name);
    return t;
}

list<tuple<var_type, string *> *> *append_to_param_list(list<tuple<var_type, string *> *> *l, tuple<var_type, string *> *t)
{
    if (l == NULL)
        l = new list<tuple<var_type, string *> *>();
    l->push_back(t);
    return l;
}

StatementList *add_to_statement_list(StatementList *sl, Statement *s)
{
    if (sl == NULL)
        sl = new StatementList();
    if (s != NULL)
        sl->l.push_back(s);
    return sl;
}

var_type check_undeclared_var(string id)
{
    for (auto it = scope_stack.rbegin(); it != scope_stack.rend(); it++)
    {
        if ((*it)->find(id) != (*it)->end())
        {
            return (**it)[id];
        }
    }

    cerr << "            Description: Variable " << id << "_ has not been declared\n";
    exit(1);
}

void check_redeclared_var(string id)
{
    if (scope_stack.back()->find(id) != scope_stack.back()->end())
    {
        cerr << "            Description: Variable is declared twice in the same scope\n";
        exit(1);
    }
}

map<string, var_type> *save_and_get_stack_top()
{
    map<string, var_type> *top_scope = new map<string, var_type>();
    *top_scope = *(scope_stack.back());
    return top_scope;
}

void pop_scope()
{
    assert(scope_stack.size() > 1);
    delete scope_stack.back();
    scope_stack.pop_back();
}

// ----------------------------------------------------------------

string *create_ID(string *name)
{
    // [ ] TODO: need to fix for general case
    if (*name == "main")
    {
        cerr << "            Description: Variable main coincides with a procedure name\n";
        exit(1);
    }

    check_redeclared_var(*name);
    (*(scope_stack.back()))[*name] = INTEGER_V; // setting default var_type before assign_type_to_decl_list() changes it
    return name;
}

Constant *create_CONST(string *val, var_type t)
{
    if (t == STRING_V)
        add_global_string(val);
    Constant *c = new Constant(*val, t);
    return c;
}

Expression *process_ID(string *name)
{
    string *x = new string(*name + "_");
    var_type t = check_undeclared_var(*name);
    Expression *e = new Expression(x, t);
    return e;
}

Expression *process_CONST(Constant *cst)
{
    Expression *e = new Expression(&(cst->val), cst->t);
    e->is_const = true;
    return e;
}

list<string *> *add_to_decl_list(list<string *> *l, string *x)
{
    if (l == NULL)
        l = new list<string *>();
    l->push_back(x);
    return l;
}

void assign_type_to_decl_list(var_type t, list<string *> *l)
{
    if (t == VOID_V)
    {
        cerr << "            Description: Variables cannot have a void type\n";
        exit(1);
    }
    for (auto it = l->begin(); it != l->end(); ++it)
        if (*it != NULL)
            (*(scope_stack.back()))[*(*it)] = t;
}

Expression *process_unary_exp(op_type op, Expression *lhs)
{
    // parsing error for other op_type values, also includes NULL case for lhs

    Expression *z;
    if (op == NOT_O)
    {
        if (lhs->t != BOOL_V)
        {
            cerr << "            Description: Wrong type for operand in Boolean expression\n";
            exit(1);
        }
        else
            z = new Expression(NULL, lhs->t, op, BOOLEAN_E, lhs);
    }
    else if (op == UMINUS_O)
    {
        if (!(lhs->t == FLOAT_V || lhs->t == INTEGER_V))
        {
            cerr << "            Description: Wrong type of operand in UMINUS expression\n";
            exit(1);
        }
        else
            z = new Expression(NULL, lhs->t, op, ARITHMETIC_E, lhs);
    }
    return z;
}

Expression *process_binary_exp(Expression *lhs, op_type op, Expression *rhs)
{
    // parsing error for UMINUS and NOT, also includes NULL cases for lhs and rhs

    map<op_type, string> error_print = {
        {PLUS_O, "PLUS"},
        {MINUS_O, "MINUS"},
        {MULT_O, "MULT"},
        {DIV_O, "DIV"}};

    if (lhs->t != rhs->t)
    {
        if (op == PLUS_O || op == MINUS_O || op == MULT_O || op == DIV_O)
            cerr << "            Description: Wrong type of operand in " << error_print[op] << " expression\n";
        else if (op == LT_O || op == LE_O || op == GT_O || op == GE_O || op == NE_O || op == EQ_O)
            cerr << "            Description: Wrong type of operand in Relational expression\n";
        else if (op == AND_O || op == OR_O)
            cerr << "            Description: Wrong type of operand in Boolean expression\n";
        exit(1);
    }
    else
    {
        if (op == PLUS_O || op == MINUS_O || op == MULT_O || op == DIV_O)
        {
            if (!(lhs->t == INTEGER_V || lhs->t == FLOAT_V))
            {
                cerr << "            Description: Wrong type of operand in " << error_print[op] << " expression\n";
                exit(1);
            }
        }
        else if (op == LT_O || op == LE_O || op == GT_O || op == GE_O || op == NE_O || op == EQ_O)
        {
            if (!(lhs->t == INTEGER_V || lhs->t == FLOAT_V))
            {
                cerr << "            Description: Wrong type of operand in Relational expression\n";
                exit(1);
            }
        }
        else if (op == AND_O || op == OR_O)
        {
            if (!(lhs->t == BOOL_V))
            {
                cerr << "            Description: Wrong type of operand in Boolean expression\n";
                exit(1);
            }
        }
    }

    Expression *z;
    if (op == PLUS_O || op == MINUS_O || op == MULT_O || op == DIV_O)
        z = new Expression(NULL, lhs->t, op, ARITHMETIC_E, lhs, rhs);
    else if (op == LT_O || op == LE_O || op == GT_O || op == GE_O || op == NE_O || op == EQ_O)
        z = new Expression(NULL, BOOL_V, op, RELATIONAL_E, lhs, rhs);
    else if (op == AND_O || op == OR_O)
        z = new Expression(NULL, BOOL_V, op, BOOLEAN_E, lhs, rhs);
    return z;
}

Expression *process_ternary_exp(Expression *ternary, Expression *lhs, Expression *rhs)
{
    // parsing error for NULL cases for lhs, rhs, and ternary

    if (lhs->t != rhs->t)
    {
        cerr << "            Description: Different data types of the two operands of conditional ast\n";
        exit(1);
    }
    if (ternary->t != BOOL_V)
    {
        cerr << "            Description: Wrong type of condition in conditional ast\n";
        exit(1);
    }

    Expression *z = new Expression(NULL, lhs->t, PLUS_O, ARITHMETIC_E, lhs, rhs, ternary); // PLUS and ARITHMETIC_E are just dummy
    return z;
}

Statement *process_assign(string *name, Expression *rhs)
{
    var_type t = check_undeclared_var(*name);
    if (t != rhs->t)
    {
        cerr << "            Description: Assignment statement data type not compatible\n";
        exit(1);
    }

    Statement *s = new Statement(name, rhs, ASSIGN_S);
    return s;
}

Statement *process_print(Expression *exp)
{
    if (exp->t == BOOL_V)
    {
        cerr << "            Description: A bool variable is not allowed in a Print Stmt\n";
        exit(1);
    }

    Statement *s = new Statement(NULL, exp, PRINT_S, exp->t);
    return s;
}

Statement *process_read(string *name)
{
    var_type v = check_undeclared_var(*name);

    if (!(v == INTEGER_V || v == FLOAT_V))
    {
        cerr << "            Description: Only Int and Float variables are allowed in a Read Stmt\n";
        exit(1);
    }

    Statement *s = new Statement(name, NULL, READ_S, v);
    return s;
}

Statement *process_if_else(Expression *if_e, Statement *then_s, Statement *else_s)
{
    if (if_e->t != BOOL_V)
    {
        cerr << "IF_E not bool\n";
        exit(1);
    }

    Statement *s = new Statement(NULL, if_e, IF_ELSE_S, INTEGER_V, then_s, else_s);
    return s;
}

Statement *process_if(Expression *if_e, Statement *then_s)
{
    if (if_e->t != BOOL_V)
    {
        cerr << "IF_E not bool\n";
        exit(1);
    }

    Statement *s = new Statement(NULL, if_e, IF_S, INTEGER_V, then_s);
    return s;
}

Statement *process_do_while(Statement *do_s, Expression *while_e)
{
    if (while_e->t != BOOL_V)
    {
        cerr << "WHILE_E not bool\n";
        exit(1);
    }

    Statement *s = new Statement(NULL, while_e, DO_WHILE_S, INTEGER_V, do_s);
    return s;
}

Statement *process_while(Expression *while_e, Statement *do_s)
{
    if (while_e->t != BOOL_V)
    {
        cerr << "WHILE_E not bool\n";
        exit(1);
    }

    Statement *s = new Statement(NULL, while_e, WHILE_S, INTEGER_V, do_s);
    return s;
}

Statement *process_comp(StatementList *sl)
{
    Statement *s;
    if (sl == NULL)
        s = new Statement(NULL, NULL, COMP_S, INTEGER_V, NULL, NULL, NULL);
    else
        s = new Statement(NULL, NULL, COMP_S, INTEGER_V, NULL, NULL, &(sl->l));
    return s;
}

// --------------------------------

void init()
{
    if (!stop_after_ast)
    {
        for (string *elem : func_ordering)
        {
            funcs[*elem]->gen_tac();
        }
    }

    if (!stop_after_tac)
    {
        for (string *elem : func_ordering)
        {
            funcs[*elem]->gen_rtl();
        }
    }

    if (show_ast)
        print_program_ast();
    if (show_tac)
        print_program_tac();
    if (show_rtl)
        print_program_rtl();
}

// --------------------------------

void print_program_ast()
{
    int fd;
    size_t len_x = strlen(filename);
    size_t len_y = len_x + strlen(".ast") + 1;
    char *new_filename = (char *)malloc(len_y * sizeof(char));
    strcpy(new_filename, filename);
    strcat(new_filename, ".ast");

    int stdout_orig;
    if (!demo_mode)
    {
        if ((fd = open(new_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
        {
            cerr << "Error creating .ast file\n";
            exit(1);
        }
        stdout_orig = dup(STDOUT_FILENO);
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            cerr << "Error redirecting output to .ast file\n";
            close(fd);
            exit(1);
        }
    }

    for (string *elem : func_ordering)
    {
        Function *f = funcs[*elem];
        f->print_func_ast();
    }

    cout << flush;
    if (!demo_mode)
    {
        dup2(stdout_orig, STDOUT_FILENO);
        close(fd);
    }
}

void print_program_tac()
{
    int fd;
    size_t len_x = strlen(filename);
    size_t len_y = len_x + strlen(".tac") + 1;
    char *new_filename = (char *)malloc(len_y * sizeof(char));
    strcpy(new_filename, filename);
    strcat(new_filename, ".tac");

    int stdout_orig;
    if (!demo_mode)
    {
        if ((fd = open(new_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
        {
            cerr << "Error creating .tac file\n";
            exit(1);
        }
        stdout_orig = dup(STDOUT_FILENO);
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            cerr << "Error redirecting output to .tac file\n";
            close(fd);
            exit(1);
        }
    }

    for (string *elem : func_ordering)
    {
        Function *f = funcs[*elem];
        f->print_func_tac();
    }

    cout << flush;
    if (!demo_mode)
    {
        dup2(stdout_orig, STDOUT_FILENO);
        close(fd);
    }
}

void print_program_rtl()
{
    int fd;
    size_t len_x = strlen(filename);
    size_t len_y = len_x + strlen(".rtl") + 1;
    char *new_filename = (char *)malloc(len_y * sizeof(char));
    strcpy(new_filename, filename);
    strcat(new_filename, ".rtl");

    int stdout_orig;
    if (!demo_mode)
    {
        if ((fd = open(new_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
        {
            cerr << "Error creating .rtl file\n";
            exit(1);
        }
        stdout_orig = dup(STDOUT_FILENO);
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            cerr << "Error redirecting output to .rtl file\n";
            close(fd);
            exit(1);
        }
    }

    for (string *elem : func_ordering)
    {
        Function *f = funcs[*elem];
        f->print_func_rtl();
    }

    cout << flush;
    if (!demo_mode)
    {
        dup2(stdout_orig, STDOUT_FILENO);
        close(fd);
    }
}

// --------------------------------

void Function::gen_tac()
{
    code = new TAC();
    if (stmts->l.size())
    {
        for (auto stmt : stmts->l)
        {
            TAC *z = stmt->gen_tac();
            if (z != NULL)
                code->l.insert(code->l.end(), z->l.begin(), z->l.end());
        }
    }
}

void Function::gen_rtl()
{
    reset_registers();
    map<string *, reg> reg_map;
    rtl_list = new list<RTL *>();

    for (auto tac_stmt : code->l)
    {
        RTL *x;
        switch (tac_stmt->t)
        {
        case NARY_TAC:
        {
            if (tac_stmt->o1 == NULL)
            { // case: UNARY
                bool is_int = tac_stmt->rhs_v == INTEGER_V || tac_stmt->rhs_v == BOOL_V;
                if (tac_stmt->o2_t != TEMP_T)
                {
                    reg r = get_free_register(is_int);
                    reg_map[tac_stmt->o2] = r;
                    iarg p1(r);
                    iarg p2(tac_stmt->o2);
                    rtl_instr i = (tac_stmt->o2_t == CONSTANT_T) ? ILOAD_R : LOAD_R;
                    x = new RTL(i, is_int, p1, p2);
                    rtl_list->push_back(x);
                }
                reg r = get_free_register(is_int);
                reg_map[tac_stmt->lhs] = r;
                iarg p1(r);
                iarg p2(reg_map[tac_stmt->o2]);
                rtl_instr i = (tac_stmt->op == NOT_O) ? NOT_R : UMINUS_R;
                x = new RTL(i, is_int, p1, p2);
                rtl_list->push_back(x);

                // Freeing the registers
                release_register(reg_map[tac_stmt->o2]);
                reg_map.erase(tac_stmt->o2);
            }
            else if (tac_stmt->o2 == NULL)
            { // case: ASSIGN
                if (tac_stmt->rhs_v == STRING_V)
                {
                    if (tac_stmt->o1_t == CONSTANT_T)
                    {
                        reg r = get_free_register(true);
                        reg_map[tac_stmt->o1] = r;
                        iarg p1(r);
                        iarg p2(get_global_string(tac_stmt->o1));
                        x = new RTL(LOADADDR_R, true, p1, p2);
                        rtl_list->push_back(x);

                        iarg p3(tac_stmt->lhs);
                        iarg p4(reg_map[tac_stmt->o1]);
                        x = new RTL(STORE_R, true, p3, p4);
                        rtl_list->push_back(x);

                        release_register(reg_map[tac_stmt->o1]);
                        reg_map.erase(tac_stmt->o1);
                    }
                    else
                    {
                        if (tac_stmt->o1_t != TEMP_T)
                        {
                            reg r = get_free_register(true);
                            reg_map[tac_stmt->o1] = r;
                            iarg p1(r);
                            iarg p2(tac_stmt->o1);
                            x = new RTL(LOAD_R, true, p1, p2);
                            rtl_list->push_back(x);
                        }
                        iarg p1(tac_stmt->lhs);
                        iarg p2(reg_map[tac_stmt->o1]);
                        x = new RTL(STORE_R, true, p1, p2);
                        rtl_list->push_back(x);

                        release_register(reg_map[tac_stmt->o1]);
                        reg_map.erase(tac_stmt->o1);
                    }
                }
                else
                {
                    bool is_int = tac_stmt->rhs_v == INTEGER_V || tac_stmt->rhs_v == BOOL_V;
                    if (tac_stmt->o1_t != TEMP_T)
                    {
                        reg r = get_free_register(is_int);
                        reg_map[tac_stmt->o1] = r;
                        iarg p1(r);
                        iarg p2(tac_stmt->o1);
                        rtl_instr i = (tac_stmt->o1_t == CONSTANT_T) ? ILOAD_R : LOAD_R;
                        x = new RTL(i, is_int, p1, p2);
                        rtl_list->push_back(x);
                    }
                    iarg p1(tac_stmt->lhs);
                    iarg p2(reg_map[tac_stmt->o1]);
                    x = new RTL(STORE_R, is_int, p1, p2);
                    rtl_list->push_back(x);

                    release_register(reg_map[tac_stmt->o1]);
                    reg_map.erase(tac_stmt->o1);
                }
            }
            else if (tac_stmt->o1 != NULL && tac_stmt->o2 != NULL)
            { // case: BINARY
                if (tac_stmt->op <= OR_O)
                { // case: arithmetic and boolean BINARY
                    bool is_int = tac_stmt->rhs_v == INTEGER_V || tac_stmt->rhs_v == BOOL_V;

                    if (tac_stmt->o1_t != TEMP_T)
                    {
                        reg r = get_free_register(is_int);
                        reg_map[tac_stmt->o1] = r;
                        iarg p1(r);
                        iarg p2(tac_stmt->o1);
                        rtl_instr i = (tac_stmt->o1_t == CONSTANT_T) ? ILOAD_R : LOAD_R;
                        x = new RTL(i, is_int, p1, p2);
                        rtl_list->push_back(x);
                    }

                    reg r_lhs = get_free_register(is_int);
                    reg_map[tac_stmt->lhs] = r_lhs;

                    if (tac_stmt->o2_t != TEMP_T)
                    {
                        reg r = get_free_register(is_int);
                        reg_map[tac_stmt->o2] = r;
                        iarg p1(r);
                        iarg p2(tac_stmt->o2);
                        rtl_instr i = (tac_stmt->o2_t == CONSTANT_T) ? ILOAD_R : LOAD_R;
                        x = new RTL(i, is_int, p1, p2);
                        rtl_list->push_back(x);
                    }

                    rtl_instr a;
                    if (tac_stmt->op == PLUS_O)
                        a = ADD_R;
                    else if (tac_stmt->op == MINUS_O)
                        a = SUB_R;
                    else if (tac_stmt->op == MULT_O)
                        a = MUL_R;
                    else if (tac_stmt->op == DIV_O)
                        a = DIV_R;
                    else if (tac_stmt->op == AND_O)
                        a = AND_R;
                    else
                        a = OR_R;
                    iarg p1(reg_map[tac_stmt->lhs]);
                    iarg p2(reg_map[tac_stmt->o1]);
                    iarg p3(reg_map[tac_stmt->o2]);
                    x = new RTL(a, is_int, p1, p2, p3);
                    rtl_list->push_back(x);
                }
                else if (tac_stmt->op <= EQ_O)
                { // case: relational BINARY
                    bool is_int = tac_stmt->rhs_v == INTEGER_V || tac_stmt->rhs_v == BOOL_V;

                    if (tac_stmt->o1_t != TEMP_T)
                    {
                        reg r = get_free_register(is_int);
                        reg_map[tac_stmt->o1] = r;
                        iarg p1(r);
                        iarg p2(tac_stmt->o1);
                        rtl_instr i = (tac_stmt->o1_t == CONSTANT_T) ? ILOAD_R : LOAD_R;
                        x = new RTL(i, is_int, p1, p2);
                        rtl_list->push_back(x);
                    }

                    if (is_int)
                    {
                        reg r_lhs = get_free_register(is_int);
                        reg_map[tac_stmt->lhs] = r_lhs;
                    }

                    if (tac_stmt->o2_t != TEMP_T)
                    {
                        reg r = get_free_register(is_int);
                        reg_map[tac_stmt->o2] = r;
                        iarg p1(r);
                        iarg p2(tac_stmt->o2);
                        rtl_instr i = (tac_stmt->o2_t == CONSTANT_T) ? ILOAD_R : LOAD_R;
                        x = new RTL(i, is_int, p1, p2);
                        rtl_list->push_back(x);
                    }

                    rtl_instr a;
                    if (tac_stmt->op == LT_O)
                        a = SLT_R;
                    else if (tac_stmt->op == LE_O)
                        a = SLE_R;
                    else if (tac_stmt->op == GT_O)
                        if (is_int)
                            a = SGT_R;
                        else
                            a = SLE_R;
                    else if (tac_stmt->op == GE_O)
                        if (is_int)
                            a = SGE_R;
                        else
                            a = SLT_R;
                    else if (tac_stmt->op == NE_O)
                        if (is_int)
                            a = SNE_R;
                        else
                            a = SEQ_R;
                    else
                        a = SEQ_R;
                    iarg *p1;
                    if (is_int)
                        p1 = new iarg(reg_map[tac_stmt->lhs]);
                    else
                        p1 = new iarg(NULL);
                    iarg p2(reg_map[tac_stmt->o1]);
                    iarg p3(reg_map[tac_stmt->o2]);
                    if (is_int)
                    {
                        x = new RTL(a, is_int, *p1, p2, p3);
                    }
                    else
                    {
                        x = new RTL(a, is_int, p2, p3, *p1);
                    }
                    rtl_list->push_back(x);

                    if (!is_int)
                    {
                        if (!is_reg_free(V0))
                        {
                            reg r1 = get_free_register(true);
                            reg_map[tac_stmt->lhs] = r1;
                            iarg p1(r1);
                            iarg p2(V0);
                            x = new RTL(MOVE_R, true, p1, p2);
                            rtl_list->push_back(x);
                            release_register(V0);
                        }

                        iarg p1(get_free_register(true)); // V0
                        iarg p2(new string("1"));
                        x = new RTL(ILOAD_R, true, p1, p2);
                        rtl_list->push_back(x);

                        reg r2 = get_free_register(true); // (dynamic)
                        reg_map[tac_stmt->lhs] = r2;
                        iarg p3(r2);
                        iarg p4(new string("zero"));
                        x = new RTL(MOVE_R, true, p3, p4);
                        rtl_list->push_back(x);

                        iarg p5(new string("0"));
                        rtl_instr q = (tac_stmt->op == GT_O || tac_stmt->op == GE_O || tac_stmt->op == NE_O) ? MOVF_R : MOVT_R;
                        x = new RTL(q, true, p3, p1, p5);
                        rtl_list->push_back(x);

                        release_register(V0);
                    }
                }

                release_register(reg_map[tac_stmt->o1]);
                reg_map.erase(tac_stmt->o1);
                release_register(reg_map[tac_stmt->o2]);
                reg_map.erase(tac_stmt->o2);
            }
            else
            {
                cerr << "Unexpected case in NARY_TAC\n";
                exit(1);
            }
            break;
        }
        case LABEL_TAC:
        {
            iarg p(tac_stmt->lhs);
            x = new RTL(LABEL_R, true, p);
            rtl_list->push_back(x);
            break;
        }
        case GOTO_TAC:
        {
            iarg p(tac_stmt->lhs);
            x = new RTL(GOTO_R, true, p);
            rtl_list->push_back(x);
            break;
        }
        case IFGOTO_TAC:
        {
            bool is_int = tac_stmt->lhs_v == INTEGER_V || tac_stmt->lhs_v == BOOL_V;
            if (tac_stmt->lhs_t == SOURCE_T)
            {
                reg r = get_free_register(is_int);
                reg_map[tac_stmt->lhs] = r;
                iarg p1(r);
                iarg p2(tac_stmt->lhs);
                x = new RTL(LOAD_R, is_int, p1, p2);
                rtl_list->push_back(x);
            }
            iarg p1(reg_map[tac_stmt->lhs]);
            iarg p2(tac_stmt->o1);
            x = new RTL(BGTZ_R, is_int, p1, p2);
            rtl_list->push_back(x);

            // Freeing the registers
            release_register(reg_map[tac_stmt->lhs]);
            reg_map.erase(tac_stmt->lhs);
            break;
        }
        case WRITE_TAC:
        {
            if (!is_reg_free(V0))
            {
                reg r1 = get_free_register(true);
                reg_map[tac_stmt->lhs] = r1;
                iarg p1(r1);
                iarg p2(V0);
                x = new RTL(MOVE_R, true, p1, p2);
                rtl_list->push_back(x);
                release_register(V0);
            }

            string *v2;
            switch (tac_stmt->lhs_v)
            {
            case INTEGER_V:
                v2 = new string("1");
                break;
            case BOOL_V:
                v2 = new string("1");
                break;
            case STRING_V:
                v2 = new string("4");
                break;
            case FLOAT_V:
                v2 = new string("3");
                break;
            }

            iarg p1(get_free_register(true)); // V0
            iarg p2(v2);
            x = new RTL(ILOAD_R, true, p1, p2);
            rtl_list->push_back(x);

            bool is_int = !(tac_stmt->lhs_v == FLOAT_V);
            reg r = is_int ? A0 : F12;
            iarg p3(r);
            if (tac_stmt->lhs_t == SOURCE_T || tac_stmt->lhs_t == STEMP_T)
            {
                iarg p4(tac_stmt->lhs);
                x = new RTL(LOAD_R, is_int, p3, p4);
                rtl_list->push_back(x);
            }
            else if (tac_stmt->lhs_t == TEMP_T)
            {
                iarg p4(reg_map[tac_stmt->lhs]);
                x = new RTL(MOVE_R, is_int, p3, p4);
                rtl_list->push_back(x);
                release_register(reg_map[tac_stmt->lhs]);
                reg_map.erase(tac_stmt->lhs);
            }
            else if (tac_stmt->lhs_t == CONSTANT_T)
            {
                switch (tac_stmt->lhs_v)
                {
                case INTEGER_V:
                {
                    reg r = get_free_arg_register();
                    iarg p1(r);
                    iarg p2(tac_stmt->lhs);
                    x = new RTL(ILOAD_R, true, p1, p2);
                    rtl_list->push_back(x);

                    release_register(r);
                    break;
                }
                case STRING_V:
                {
                    reg r = get_free_arg_register();
                    iarg p1(r);
                    iarg p2(get_global_string(tac_stmt->lhs));
                    x = new RTL(LOADADDR_R, true, p1, p2);
                    rtl_list->push_back(x);

                    release_register(r);
                    break;
                }
                case FLOAT_V:
                {
                    reg r = F12;
                    iarg p1(r);
                    iarg p2(tac_stmt->lhs);
                    x = new RTL(ILOAD_R, false, p1, p2);
                    rtl_list->push_back(x);

                    release_register(r);
                    break;
                }
                }
            }
            else
            {
                cerr << "Unexpected case in WRITE_TAC\n";
                exit(1);
            }

            x = new RTL(WRITE_R);
            rtl_list->push_back(x);

            // Freeing the registers
            release_register(V0);
            release_register(r);
            break;
        }
        case READ_TAC:
        {
            reg r = get_free_register(true);
            if (r != V0)
            { // V0 (SYSCALL)
                cout << "UNEXPECTED READ_TAC case in gen_rtl\n";
                break;
            }
            bool is_int = tac_stmt->lhs_v == INTEGER_V || tac_stmt->lhs_v == BOOL_V;

            iarg p1(r);
            iarg p2(new string(to_string(is_int ? 5 : 7)));
            x = new RTL(ILOAD_R, true, p1, p2);
            rtl_list->push_back(x);
            x = new RTL(READ_R);
            rtl_list->push_back(x);

            iarg p3(tac_stmt->lhs);
            r = is_int ? V0 : F0;
            iarg p4(r);
            x = new RTL(STORE_R, is_int, p3, p4);
            rtl_list->push_back(x);

            // Freeing the registers
            release_register(V0);
            release_register(r);
            break;
        }
        }
    }
}

void Function::print_func_ast()
{
    cout << "**PROCEDURE: " << *name << "\n";
    cout << "	Return Type: " << print_var_type[return_type] << "\n";
    cout << "	Formal Parameters:\n";
    if (params != NULL)
    {
        for (tuple<var_type, string *> *param : *params)
        {
            cout << "		" << *get<1>(*param) << "_  Type:" << print_var_type[get<0>(*param)] << "\n";
        }
    }
    cout << "**BEGIN: Abstract Syntax Tree";
    for (auto stmt : stmts->l)
    {
        stmt->print_stmt_ast(0);
    }
    cout << "\n**END: Abstract Syntax Tree\n";
}

void Function::print_func_tac()
{
    if (stmts->l.size() == 0)
        return;
    cout << "**PROCEDURE: " << *name << "\n";
    cout << "**BEGIN: Three Address Code Statements\n";
    for (auto tac_stmt : code->l)
    {
        tac_stmt->print_tac();
    }
    cout << "**END: Three Address Code Statements\n";
}

void Function::print_func_rtl()
{
    if (rtl_list->size() == 0)
        return;
    cout << "**PROCEDURE: " << *name << "\n";
    cout << "**BEGIN: RTL Statements";
    for (auto rtl_stmt : *rtl_list)
    {
        rtl_stmt->print_rtl();
    }
    cout << "\n**END: RTL Statements\n";
}

// --------------------------------

// +1 depth == 2 spaces
// DEPTH starts from 0 for statements, and 1 for assignments

void Expression::print_exp_ast(int depth = 1)
{
    string space = "         ";
    for (int i = 0; i < depth; i++)
        space += "  ";

    if (lhs == NULL && rhs == NULL)
    { // CASE: id, constant
        if (!is_const)
            cout << "Name : " << *place << print_var_type[t];
        else
        {
            if (t == STRING_V)
                cout << "String : " << *place << print_var_type[t];
            else if (t == INTEGER_V || t == FLOAT_V)
                cout << "Num : " << *place << print_var_type[t];
            else
                cout << "Name : " << *place << print_var_type[t];
        }
    }
    else if (lhs != NULL && rhs == NULL)
    { // CASE: unary
        if (e == BOOLEAN_E)
        {
            cout << "\n"
                 << space << "Condition: NOT<bool>";
            cout << "\n"
                 << space << "  L_Opd (";
            lhs->print_exp_ast(depth + 2);
            cout << ")";
        }
        else if (e == ARITHMETIC_E)
        {
            cout << "\n"
                 << space << "Arith: Uminus" << print_var_type[lhs->t];
            cout << "\n"
                 << space << "  L_Opd (";
            lhs->print_exp_ast(depth + 2);
            cout << ")";
        }
    }
    else if (ternary != NULL)
    { // CASE: ternary
        ternary->print_exp_ast(depth + 2);
        cout << "\n"
             << space << "      True_Part (";
        lhs->print_exp_ast(depth + 4);
        cout << ")\n"
             << space << "      False_Part (";
        rhs->print_exp_ast(depth + 4);
        cout << ")";
    }
    else
    {
        if (e == ARITHMETIC_E)
            cout << "\n"
                 << space << "Arith: " << op_print[o];
        else
            cout << "\n"
                 << space << "Condition: " << op_print[o];

        if (e == RELATIONAL_E)
            cout << print_var_type[BOOL_V];
        else
            cout << print_var_type[lhs->t];

        cout << "\n"
             << space << "  L_Opd (";
        lhs->print_exp_ast(depth + 2);
        cout << ")\n"
             << space << "  R_Opd (";
        rhs->print_exp_ast(depth + 2);
        cout << ")";
    }
}

void Statement::print_stmt_ast(int depth = 0)
{
    string space = "         ";
    for (int i = 0; i < depth; i++)
        space += "  ";

    switch (t)
    {
    case ASSIGN_S:
        cout << "\n"
             << space << "Asgn:";
        cout << "\n"
             << space << "  LHS (Name : " << *s << "_" << print_var_type[e->t] << ")";
        cout << "\n"
             << space << "  RHS (";
        e->print_exp_ast(depth + 1);
        cout << ")";
        break;
    case PRINT_S:
        cout << "\n"
             << space << "Write: ";
        e->print_exp_ast(depth);
        break;
    case READ_S:
        cout << "\n"
             << space << "Read: Name : " << *s << "_" << print_var_type[v];
        break;
    case IF_S:
        cout << "\n"
             << space << "If: \n";
        cout << space << "  Condition (";
        e->print_exp_ast(depth + 2);
        cout << ")\n";
        cout << space << "  Then (";
        s1->print_stmt_ast(depth + 1);
        cout << ")";
        break;
    case IF_ELSE_S:
        cout << "\n"
             << space << "If: \n";
        cout << space << "  Condition (";
        e->print_exp_ast(depth + 2);
        cout << ")\n";
        cout << space << "  Then (";
        s1->print_stmt_ast(depth + 1);
        cout << ")\n";
        cout << space << "  Else (";
        s2->print_stmt_ast(depth + 1);
        cout << ")";
        break;
    case DO_WHILE_S:
        cout << "\n"
             << space << "Do: \n";
        cout << space << "  Body (";
        s1->print_stmt_ast(depth + 1);
        cout << ")\n";
        cout << space << "  While Condition (";
        e->print_exp_ast(depth + 2);
        cout << ")";
        break;
    case WHILE_S:
        cout << "\n"
             << space << "While: \n";
        cout << space << "  Condition (";
        e->print_exp_ast(depth + 2);
        cout << ")\n";
        cout << space << "  Body (";
        s1->print_stmt_ast(depth + 1);
        cout << ")";
        break;
    case COMP_S:
        for (auto stmt : *sl)
            stmt->print_stmt_ast(depth + 1);
        break;
    }
}

// --------------------------------

void TAC_Statement::print_tac()
{
    switch (t)
    {
    case NARY_TAC:
        cout << "	" << *lhs << " = ";
        if (o1 != NULL)
            cout << *o1 << " ";
        if (o2 != NULL)
            cout << print_op_type[op] << " " << *o2;
        cout << endl;
        break;
    case LABEL_TAC:
        cout << *lhs << ":" << endl;
        break;
    case GOTO_TAC:
        cout << "	goto " << *lhs << endl;
        break;
    case IFGOTO_TAC:
        cout << "	if(" << *lhs << ") goto " << *o1 << endl;
        break;
    case WRITE_TAC:
        cout << "	write  " << *lhs << endl;
        break;
    case READ_TAC:
        cout << "	read  " << *lhs << endl;
        break;
    }
}

TAC *Statement::gen_tac()
{
    TAC_Statement *c1;
    string *x;
    code = new TAC();

    if (t == READ_S || t == ASSIGN_S)
        x = new string(*s + '_');

    if (t == READ_S)
    {
        c1 = new TAC_Statement(x, PLUS_O, NULL, NULL, READ_TAC, SOURCE_T, SOURCE_T, SOURCE_T, v, INTEGER_V);
        code->l.push_back(c1);
    }
    else if (t == PRINT_S)
    {
        TAC *z = e->gen_tac();
        c1 = new TAC_Statement(e->place, PLUS_O, NULL, NULL, WRITE_TAC, find_tmp_type(e->place, e->is_const), SOURCE_T, SOURCE_T, e->t, INTEGER_V);
        if (z != NULL)
            code->l = e->code->l;
        code->l.push_back(c1);
    }
    else if (t == ASSIGN_S)
    {
        TAC *z = e->gen_tac();
        c1 = new TAC_Statement(x, PLUS_O, e->place, NULL, NARY_TAC, SOURCE_T, find_tmp_type(e->place, e->is_const), SOURCE_T, e->t, e->t);
        if (z != NULL)
            code->l = e->code->l;
        code->l.push_back(c1);
    }
    else if (t == WHILE_S)
    { // [ ] TODO: check ordering of gen_tac()
        e->gen_tac();
        TAC *z = s1->gen_tac();

        string *t1 = get_new_temp();
        string *l1 = get_new_label(), *l2 = get_new_label();

        c1 = new TAC_Statement(l1, PLUS_O, NULL, NULL, LABEL_TAC);
        code->l.push_back(c1);
        if (e->code != NULL)
            code->l.insert(code->l.end(), e->code->l.begin(), e->code->l.end());
        c1 = new TAC_Statement(t1, NOT_O, NULL, e->place, NARY_TAC, TEMP_T, SOURCE_T, find_tmp_type(e->place, e->is_const), e->t, e->t);
        code->l.push_back(c1);

        c1 = new TAC_Statement(t1, PLUS_O, l2, NULL, IFGOTO_TAC, TEMP_T, SOURCE_T, SOURCE_T, BOOL_V, INTEGER_V);
        code->l.push_back(c1);

        if (z != NULL)
            code->l.insert(code->l.end(), z->l.begin(), z->l.end());

        c1 = new TAC_Statement(l1, PLUS_O, NULL, NULL, GOTO_TAC);
        code->l.push_back(c1);
        c1 = new TAC_Statement(l2, PLUS_O, NULL, NULL, LABEL_TAC);
        code->l.push_back(c1);
    }
    else if (t == DO_WHILE_S)
    { // [ ] TODO: check ordering of gen_tac()
        TAC *z = s1->gen_tac();
        e->gen_tac();

        string *l1 = get_new_label();
        c1 = new TAC_Statement(l1, PLUS_O, NULL, NULL, LABEL_TAC);
        code->l.push_back(c1);
        if (z != NULL)
            code->l.insert(code->l.end(), z->l.begin(), z->l.end());
        if (e->code != NULL)
            code->l.insert(code->l.end(), e->code->l.begin(), e->code->l.end());

        c1 = new TAC_Statement(e->place, PLUS_O, l1, NULL, IFGOTO_TAC, find_tmp_type(e->place, e->is_const), SOURCE_T, SOURCE_T, e->t, INTEGER_V);
        code->l.push_back(c1);
    }
    else if (t == IF_S)
    {
        e->gen_tac();
        TAC *z = s1->gen_tac();

        string *t1 = get_new_temp();
        string *l1 = get_new_label();

        if (e->code != NULL)
            code->l.insert(code->l.end(), e->code->l.begin(), e->code->l.end());
        c1 = new TAC_Statement(t1, NOT_O, NULL, e->place, NARY_TAC, TEMP_T, SOURCE_T, find_tmp_type(e->place, e->is_const), e->t, e->t);
        code->l.push_back(c1);

        c1 = new TAC_Statement(t1, PLUS_O, l1, NULL, IFGOTO_TAC, TEMP_T, SOURCE_T, SOURCE_T, BOOL_V, INTEGER_V);
        code->l.push_back(c1);
        if (z != NULL)
            code->l.insert(code->l.end(), z->l.begin(), z->l.end());
        c1 = new TAC_Statement(l1, PLUS_O, NULL, NULL, GOTO_TAC);
        code->l.push_back(c1);
        c1 = new TAC_Statement(l1, PLUS_O, NULL, NULL, LABEL_TAC);
        code->l.push_back(c1);
    }
    else if (t == IF_ELSE_S)
    {
        e->gen_tac();
        TAC *z1 = s1->gen_tac();

        string *t1 = get_new_temp();
        string *l1 = get_new_label(), *l2 = get_new_label();

        TAC *z2 = s2->gen_tac();

        if (e->code != NULL)
            code->l.insert(code->l.end(), e->code->l.begin(), e->code->l.end());
        c1 = new TAC_Statement(t1, NOT_O, NULL, e->place, NARY_TAC, TEMP_T, SOURCE_T, find_tmp_type(e->place, e->is_const), BOOL_V, e->t);
        code->l.push_back(c1);

        c1 = new TAC_Statement(t1, PLUS_O, l2, NULL, IFGOTO_TAC, TEMP_T, SOURCE_T, SOURCE_T, BOOL_V, INTEGER_V);
        code->l.push_back(c1);
        if (z1 != NULL)
            code->l.insert(code->l.end(), z1->l.begin(), z1->l.end());
        c1 = new TAC_Statement(l1, PLUS_O, NULL, NULL, GOTO_TAC);
        code->l.push_back(c1);
        c1 = new TAC_Statement(l2, PLUS_O, NULL, NULL, LABEL_TAC);
        code->l.push_back(c1);
        if (z2 != NULL)
            code->l.insert(code->l.end(), z2->l.begin(), z2->l.end());
        c1 = new TAC_Statement(l1, PLUS_O, NULL, NULL, LABEL_TAC);
        code->l.push_back(c1);
    }
    else if (t == COMP_S)
    {
        if (sl != NULL)
        {
            for (auto stmt : *sl)
            {
                TAC *z = stmt->gen_tac();
                if (z != NULL)
                    code->l.insert(code->l.end(), z->l.begin(), z->l.end());
            }
        }
    }
    return code;
}

TAC *Expression::gen_tac()
{
    if (lhs == NULL && rhs == NULL)
    { // CASE: id, constant
        code = NULL;
    }
    else if (lhs != NULL && rhs == NULL)
    { // CASE: unary
        lhs->gen_tac();
        string *t1 = get_new_temp();
        place = t1;
        code = new TAC();
        TAC_Statement *c3 = new TAC_Statement(t1, o, NULL, lhs->place, NARY_TAC, TEMP_T, SOURCE_T, find_tmp_type(lhs->place, lhs->is_const), lhs->t, lhs->t);
        if (lhs->code != NULL)
            code->l = lhs->code->l;
        code->l.push_back(c3);
    }
    else if (ternary != NULL)
    { // CASE: ternary
        ternary->gen_tac();
        string *t2 = get_new_stemp();
        string *l1 = get_new_label(), *l2 = get_new_label();

        lhs->gen_tac();
        rhs->gen_tac();
        string *t1 = get_new_temp();
        place = t2;

        code = new TAC();
        if (ternary->code != NULL)
            code->l = ternary->code->l;
        TAC_Statement *x = new TAC_Statement(t1, NOT_O, NULL, ternary->place, NARY_TAC, TEMP_T, SOURCE_T, find_tmp_type(ternary->place, ternary->is_const), BOOL_V, ternary->t);
        code->l.push_back(x);

        x = new TAC_Statement(t1, PLUS_O, l1, NULL, IFGOTO_TAC, TEMP_T, SOURCE_T, SOURCE_T, BOOL_V, INTEGER_V);
        code->l.push_back(x);
        if (lhs->code != NULL)
            code->l.insert(code->l.end(), lhs->code->l.begin(), lhs->code->l.end());
        x = new TAC_Statement(t2, PLUS_O, lhs->place, NULL, NARY_TAC, STEMP_T, find_tmp_type(lhs->place, lhs->is_const), SOURCE_T, lhs->t, lhs->t);
        code->l.push_back(x);
        x = new TAC_Statement(l2, PLUS_O, NULL, NULL, GOTO_TAC);
        code->l.push_back(x);
        x = new TAC_Statement(l1, PLUS_O, NULL, NULL, LABEL_TAC);
        code->l.push_back(x);
        if (rhs->code != NULL)
            code->l.insert(code->l.end(), rhs->code->l.begin(), rhs->code->l.end());
        x = new TAC_Statement(t2, PLUS_O, rhs->place, NULL, NARY_TAC, STEMP_T, find_tmp_type(rhs->place, rhs->is_const), SOURCE_T, rhs->t, rhs->t);
        code->l.push_back(x);
        x = new TAC_Statement(l2, PLUS_O, NULL, NULL, LABEL_TAC);
        code->l.push_back(x);
    }
    else
    { // CASE: binary
        code = new TAC();
        lhs->gen_tac();
        rhs->gen_tac();
        if (lhs->code != NULL)
            code->l = lhs->code->l;
        if (rhs->code != NULL)
            code->l.insert(code->l.end(), rhs->code->l.begin(), rhs->code->l.end());
        string *t1 = get_new_temp();
        place = t1;
        TAC_Statement *c3 = new TAC_Statement(t1, o, lhs->place, rhs->place, NARY_TAC, TEMP_T, find_tmp_type(lhs->place, lhs->is_const), find_tmp_type(rhs->place, rhs->is_const), lhs->t, rhs->t);
        code->l.push_back(c3);
    }
    return code;
}

void RTL::print_rtl()
{
    switch (i)
    {
    case LOAD_R:
        cout << (is_int ? "\n	load:        " : "\n	load.d:      ") << a1.to_string() << " <- " << a2.to_string();
        break;
    case ILOAD_R:
        cout << (is_int ? "\n	iLoad:       " : "\n	iLoad.d:     ") << a1.to_string() << " <- " << a2.to_string();
        break;
    case NOT_R:
        cout << "\n	not:         " << a1.to_string() << " <- " << a2.to_string();
        break;
    case AND_R:
        cout << "\n	and:         " << a1.to_string() << " <- " << a2.to_string() << " , " << a3.to_string();
        break;
    case OR_R:
        cout << "\n	or:          " << a1.to_string() << " <- " << a2.to_string() << " , " << a3.to_string();
        break;
    case BGTZ_R:
        cout << "\n	bgtz:        " << a1.to_string() << " , " << a2.to_string();
        break;
    case STORE_R:
        cout << (is_int ? "\n	store:       " : "\n	store.d:     ") << a1.to_string() << " <- " << a2.to_string();
        break;
    case GOTO_R:
        cout << "\n	goto:        " << a1.to_string();
        break;
    case LABEL_R:
        cout << "\n\n  " << a1.to_string() << ":";
        break;
    case MOVE_R:
        cout << (is_int ? "\n	move:        " : "\n	move.d:      ") << a1.to_string() << " <- " << a2.to_string();
        break;
    case MOVT_R:
        cout << "\n	movt:        " << a1.to_string() << " <- " << a2.to_string() << " , " << a3.to_string();
        break;
    case MOVF_R:
        cout << "\n	movf:        " << a1.to_string() << " <- " << a2.to_string() << " , " << a3.to_string();
        break;
    case UMINUS_R:
        cout << (is_int ? "\n	uminus:      " : "\n	uminus.d:    ") << a1.to_string() << " <- " << a2.to_string();
        break;
    case ADD_R:
        cout << (is_int ? "\n	add:         " : "\n	add.d:       ") << a1.to_string() << " <- " << a2.to_string() << " , " << a3.to_string();
        break;
    case SUB_R:
        cout << (is_int ? "\n	sub:         " : "\n	sub.d:       ") << a1.to_string() << " <- " << a2.to_string() << " , " << a3.to_string();
        break;
    case MUL_R:
        cout << (is_int ? "\n	mul:         " : "\n	mul.d:       ") << a1.to_string() << " <- " << a2.to_string() << " , " << a3.to_string();
        break;
    case DIV_R:
        cout << (is_int ? "\n	div:         " : "\n	div.d:       ") << a1.to_string() << " <- " << a2.to_string() << " , " << a3.to_string();
        break;
    case SLT_R:
        cout << (is_int ? "\n	slt:         " : "\n	slt.d:       ") << a1.to_string() << (is_int ? " <- " : " , ") << a2.to_string();
        if (is_int)
            cout << " , " << a3.to_string();
        break;
    case SLE_R:
        cout << (is_int ? "\n	sle:         " : "\n	sle.d:       ") << a1.to_string() << (is_int ? " <- " : " , ") << a2.to_string();
        if (is_int)
            cout << " , " << a3.to_string();
        break;
    case SGT_R:
        cout << (is_int ? "\n	sgt:         " : "\n	sgt.d:       ") << a1.to_string() << (is_int ? " <- " : " , ") << a2.to_string();
        if (is_int)
            cout << " , " << a3.to_string();
        break;
    case SGE_R:
        cout << (is_int ? "\n	sge:         " : "\n	sge.d:       ") << a1.to_string() << (is_int ? " <- " : " , ") << a2.to_string();
        if (is_int)
            cout << " , " << a3.to_string();
        break;
    case SNE_R:
        cout << (is_int ? "\n	sne:         " : "\n	sne.d:       ") << a1.to_string() << (is_int ? " <- " : " , ") << a2.to_string();
        if (is_int)
            cout << " , " << a3.to_string();
        break;
    case SEQ_R:
        cout << (is_int ? "\n	seq:         " : "\n	seq.d:       ") << a1.to_string() << (is_int ? " <- " : " , ") << a2.to_string();
        if (is_int)
            cout << " , " << a3.to_string();
        break;
    case READ_R:
        cout << "\n	read";
        break;
    case WRITE_R:
        cout << "\n	write";
        break;
    case LOADADDR_R:
        cout << "\n	load_addr:   " << a1.to_string() << " <- " << a2.to_string();
        break;
    }
}