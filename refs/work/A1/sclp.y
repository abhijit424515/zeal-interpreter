%{
# include <stdio.h>
# include <iostream>
# include <string.h>
using namespace std;

extern "C" void yyerror(char *s);
extern int yylex(void);
%}

%token INTEGER FLOAT VOID STRING BOOL READ WRITE ASSIGN STRING_CONSTANT DOUBLE_NUMBER INTEGER_NUMBER NAME

%left '+' '-'
%left '*' '/'
%right UMINUS

%%

program
    : global_decl_statement_list_with_func func_main
    | global_var_decl_stmt_list func_main
    | func_main
;

global_var_decl_stmt_list
    : global_var_decl_stmt_list var_decl_stmt
    | var_decl_stmt
;

global_decl_statement_list_with_func
    : global_decl_statement_list_with_func var_decl_stmt
    | global_var_decl_stmt_list func_decl
    | func_decl
;

func_decl
    : func_header '(' formal_param_list ')' ';'
    | func_header '(' ')' ';'
;

func_header
    : named_type NAME
;

func_main
    : func_header '(' formal_param_list ')' '{' optional_local_var_decl_stmt_list statement_list '}'
    | func_header '(' ')' '{' optional_local_var_decl_stmt_list statement_list '}'
;

formal_param_list
    : formal_param_list ',' formal_param
    | formal_param
;

formal_param
    : param_type NAME
;

param_type
    : INTEGER
    | FLOAT
    | BOOL
    | STRING
;

statement_list
    : statement_list statement
    | %empty
;

statement
    : assignment_statement
    | read_statement
    | print_statement
;

optional_local_var_decl_stmt_list
    : %empty
    | var_decl_stmt_list
;

var_decl_stmt_list
    : var_decl_stmt
    | var_decl_stmt_list var_decl_stmt
;

var_decl_stmt
    : named_type var_decl_item_list ';'
;

var_decl_item_list
    : var_decl_item_list ',' var_decl_item
    | var_decl_item
;

var_decl_item
    : NAME
;

named_type
    : INTEGER
    | FLOAT
    | VOID
    | STRING
    | BOOL
;

assignment_statement
    : variable_as_operand ASSIGN expression ';'
;

print_statement
    : WRITE expression ';'
;

read_statement
    : READ variable_name ';'
;

expression
    : expression '+' expression
    | expression '-' expression
    | expression '*' expression
    | expression '/' expression
    | '-' expression
    | '(' expression ')'
    | variable_as_operand
    | constant_as_operand
;
 
variable_as_operand
    : variable_name
;

variable_name
    : NAME
;

constant_as_operand
    : INTEGER_NUMBER
    | DOUBLE_NUMBER
    | STRING_CONSTANT
;





%%