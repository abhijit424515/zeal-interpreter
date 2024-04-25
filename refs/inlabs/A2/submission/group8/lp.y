%{
#include "common-headers.hh"
#include "code.hh"
extern "C" void yyerror(char *s);
extern int yylex(void);

%}
%union{
	string * name;
	Expr_Attribute * expression_attributes;
	Code *code;
}
%token <name> NUM 
%token <name> ID
%left '+' '-'
%left '*' '/'
%right Uminus
%type <expression_attributes> Expr
%type <code> Stmt
%type <code> StmtList

%start Start
%%

Start : StmtList 			{ 
      						process_finish($1); 
      					}
	;

StmtList : Stmt 			{ $$ = process_Stmt($1); }
	| StmtList Stmt 		{ $$ = process_Stmt_List($1,$2); }
	;

Stmt : ID '=' Expr  ';' 		{ $$ = process_Asgn($1, $3); }
	| Expr ';' 			{ $$ = process_Asgn(NULL, $1); }
	;

Expr : Expr '+' Expr			{ $$ = process_Expr($1, PLUS, $3); }
	| Expr '*' Expr 		{ $$ = process_Expr($1, MULT, $3); }
	| Expr '/' Expr 		{ $$ = process_Expr($1, DIV, $3); }
	| Expr '-' Expr 		{ $$ = process_Expr($1, MINUS, $3); }
	| '-' Expr	%prec Uminus	{ $$ = process_Expr($2, UMINUS, NULL); }
	| NUM 				{ $$ = process_NUM($1); }
	| ID 				{ $$ = process_ID($1); }
	;
	
%%
