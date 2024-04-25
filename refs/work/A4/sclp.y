
%{
# include "common-headers.hh"

using namespace std;
extern "C" void yyerror(char *s);
extern int yylex(void);
extern bool stop_after_parse;
extern bool stop_after_ast;
%}

%union{
    string * name;
    Expression *exp;
    Constant *cst;
    var_type v;
    list<string*> *name_list;
    Statement *stm;
    StatementList *stm_list;
    map<string,var_type> *map_pointer;
    tuple<var_type,string*> *id_tuple;
    list<tuple<var_type,string*>*> *id_tuple_list;
    Function *fun;
}

%token INTEGER FLOAT VOID STRING BOOL READ WRITE ASSIGN NOT EQ NE GT LT AND GE LE OR IF ELSE WHILE DO
%token <name> NAME STRING_CONSTANT DOUBLE_NUMBER INTEGER_NUMBER

%right '?' ':'
%left OR
%left AND
%right NOT
%nonassoc LE GE LT GT EQ NE
%left '+' '-'
%left '*' '/'
%right UMINUS
%nonassoc IF
%nonassoc ELSE

%type <v> named_type param_type;
%type <exp> expression rel_expression if_condition;
%type <cst> constant_as_operand;
%type <name> var_decl_item variable_name variable_as_operand;
%type <name_list> var_decl_item_list;
%type <stm> statement assignment_statement if_statement do_while_statement while_statement compound_statement read_statement print_statement;
%type <stm_list> statement_list;
%type <map_pointer> optional_local_var_decl_stmt_list;
%type <id_tuple> func_header formal_param;
%type <id_tuple_list> formal_param_list;
%type <fun> func_def; 
%start program;

%%

program
    : global_decl_statement_list_with_func func_def             { if (!stop_after_parse) init(); }
    | global_var_decl_stmt_list func_def                        { if (!stop_after_parse) init(); }
    | func_def                                                  { if (!stop_after_parse) init(); }
;

global_decl_statement_list_with_func
    : global_decl_statement_list_with_func var_decl_stmt        { }
    | global_var_decl_stmt_list func_decl                       { }
    | func_decl                                                 { }
;

global_var_decl_stmt_list
    : global_var_decl_stmt_list var_decl_stmt       { }
    | var_decl_stmt                                 { }
;

func_decl
    : func_header '(' formal_param_list ')' ';'     { if (!stop_after_parse) fn_decl($1, $3); }
    | func_header '(' ')' ';'                       { if (!stop_after_parse) fn_decl($1, NULL); }
;

func_header
    : named_type NAME                               { if (!stop_after_parse) $$ = fn_header($1, $2); }
;

func_def
    : func_header '(' formal_param_list ')' '{' optional_local_var_decl_stmt_list statement_list '}'    { if (!stop_after_parse) $$ = fn_defn($1, $3, $6, $7); }
    | func_header '(' ')' '{' optional_local_var_decl_stmt_list statement_list '}'                      { if (!stop_after_parse) $$ = fn_defn($1, NULL, $5, $6); }
;

formal_param_list
    : formal_param_list ',' formal_param            { if (!stop_after_parse) $$ = append_to_param_list($1,$3); }
    | formal_param                                  { if (!stop_after_parse) $$ = append_to_param_list(NULL,$1); }
;

formal_param
    : param_type NAME                               { if (!stop_after_parse) $$ = add_param($1, $2); }
;

param_type
    : INTEGER                                       { if (!stop_after_parse) $$ = INTEGER_V; }
    | FLOAT                                         { if (!stop_after_parse) $$ = FLOAT_V; }
    | BOOL                                          { if (!stop_after_parse) $$ = BOOL_V; }
    | STRING                                        { if (!stop_after_parse) $$ = STRING_V; }
;

statement_list
    : statement_list statement                      { if (!stop_after_parse) $$ = add_to_statement_list($1, $2); }
    | %empty                                        { if (!stop_after_parse) $$ = add_to_statement_list(NULL, NULL); }
;

statement
    : assignment_statement                          { if (!stop_after_parse) $$ = $1; }
    | if_statement                                  { if (!stop_after_parse) $$ = $1; }
    | do_while_statement                            { if (!stop_after_parse) $$ = $1; }
    | while_statement                               { if (!stop_after_parse) $$ = $1; }
    | compound_statement                            { if (!stop_after_parse) $$ = $1; }
    | read_statement                                { if (!stop_after_parse) $$ = $1; }
    | print_statement                               { if (!stop_after_parse) $$ = $1; }
;

optional_local_var_decl_stmt_list
    : %empty                                        { if (!stop_after_parse) $$ = save_and_get_stack_top(); }
    | var_decl_stmt_list                            { if (!stop_after_parse) $$ = save_and_get_stack_top(); }
;

var_decl_stmt_list
    : var_decl_stmt_list var_decl_stmt              { }
    | var_decl_stmt                                 { }
;

var_decl_stmt
    : named_type var_decl_item_list ';'             { if (!stop_after_parse) assign_type_to_decl_list($1, $2); }
;

var_decl_item_list
    : var_decl_item_list ',' var_decl_item          { if (!stop_after_parse) $$ = add_to_decl_list($1, $3); }
    | var_decl_item                                 { if (!stop_after_parse) $$ = add_to_decl_list(NULL, $1); }
;

var_decl_item
    : NAME                                          { if (!stop_after_parse) $$ = create_ID($1); }
;

named_type
    : INTEGER                                       { if (!stop_after_parse) $$ = INTEGER_V; }
    | FLOAT                                         { if (!stop_after_parse) $$ = FLOAT_V; }
    | VOID                                          { if (!stop_after_parse) $$ = VOID_V; }               
    | STRING                                        { if (!stop_after_parse) $$ = STRING_V; }
    | BOOL                                          { if (!stop_after_parse) $$ = BOOL_V; }         
;

assignment_statement
    : variable_as_operand ASSIGN expression ';'     { if (!stop_after_parse) $$ = process_assign($1, $3); }
;

if_condition
    : '(' expression ')'                            { if (!stop_after_parse) $$ = $2; }
;

if_statement
    : IF if_condition statement ELSE statement      { if (!stop_after_parse) $$ = process_if_else($2,$3,$5); }
    | IF if_condition statement                     { if (!stop_after_parse) $$ = process_if($2,$3); }
;

do_while_statement
    : DO statement WHILE '(' expression ')' ';'     { if (!stop_after_parse) $$ = process_do_while($2,$5); }
;

while_statement
    : WHILE '(' expression ')' statement            { if (!stop_after_parse) $$ = process_while($3,$5); }
;

compound_statement
    : '{' statement_list '}'                        { if (!stop_after_parse) $$ = process_comp($2); }
;

print_statement
    : WRITE expression ';'                          { if (!stop_after_parse) $$ = process_print($2); }
;

read_statement
    : READ variable_name ';'                        { if (!stop_after_parse) $$ = process_read($2); }
;

expression
    : expression '+' expression                     { if (!stop_after_parse) $$ = process_binary_exp($1, PLUS_O, $3); }
    | expression '-' expression                     { if (!stop_after_parse) $$ = process_binary_exp($1, MINUS_O, $3); }
    | expression '*' expression                     { if (!stop_after_parse) $$ = process_binary_exp($1, MULT_O, $3); }
    | expression '/' expression                     { if (!stop_after_parse) $$ = process_binary_exp($1, DIV_O, $3); }
    | '-' expression          %prec UMINUS          { if (!stop_after_parse) $$ = process_unary_exp(UMINUS_O, $2); }
    | '(' expression ')'                            { if (!stop_after_parse) $$ = $2; }
    | expression '?' expression ':' expression      { if (!stop_after_parse) $$ = process_ternary_exp($1, $3, $5); }
    | expression AND expression                     { if (!stop_after_parse) $$ = process_binary_exp($1, AND_O, $3); }
    | expression OR expression                      { if (!stop_after_parse) $$ = process_binary_exp($1, OR_O, $3); }
    | NOT expression                                { if (!stop_after_parse) $$ = process_unary_exp(NOT_O, $2); }
    | rel_expression                                { if (!stop_after_parse) $$ = $1; }
    | variable_as_operand                           { if (!stop_after_parse) $$ = process_ID($1); }
    | constant_as_operand                           { if (!stop_after_parse) $$ = process_CONST($1); }
;

rel_expression
    : expression LT expression                      { if (!stop_after_parse) $$ = process_binary_exp($1, LT_O, $3); }
    | expression LE expression                      { if (!stop_after_parse) $$ = process_binary_exp($1, LE_O, $3); }
    | expression GT expression                      { if (!stop_after_parse) $$ = process_binary_exp($1, GT_O, $3); }
    | expression GE expression                      { if (!stop_after_parse) $$ = process_binary_exp($1, GE_O, $3); }
    | expression NE expression                      { if (!stop_after_parse) $$ = process_binary_exp($1, NE_O, $3); }
    | expression EQ expression                      { if (!stop_after_parse) $$ = process_binary_exp($1, EQ_O, $3); }
;
 
variable_as_operand
    : variable_name                                 { if (!stop_after_parse) $$ = $1; }
;

variable_name
    : NAME                                          { if (!stop_after_parse) $$ = $1; }
;

constant_as_operand
    : INTEGER_NUMBER                                { if (!stop_after_parse) $$ = create_CONST($1, INTEGER_V); }
    | DOUBLE_NUMBER                                 { if (!stop_after_parse) $$ = create_CONST($1, FLOAT_V); }
    | STRING_CONSTANT                               { if (!stop_after_parse) $$ = create_CONST($1, STRING_V); }
;

%%