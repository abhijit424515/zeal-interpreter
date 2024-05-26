%{
	#include <cstdio>

	extern "C" void yyerror(char *s);
	extern int yylex(void);
%}

%token READ WRITE ASSIGN COMMA LRB RRB LCB RCB PLUS MINUS MULT DIV NEWLINE STR_CONST FLT_CONST INT_CONST NAME COMMENT LOOP INDENT

%%

program
	:	stmt_list
;

loop
	:	LOOP NEWLINE idt_stmt_list						// stmt_list will store the indent of the compound statements
;

idt_stmt_list
	:	INDENT stmt_list
;

stmt_list
	:	stmt_list stmt										// ensure that all stmts have same indent, else 
	|	%empty
;

stmt
	:	INDENT decl NEWLINE											{ printf("TB: %d\n", $1); }
	| NEWLINE
	// |	loop
;

decl
	:	NAME ASSIGN constant
;

constant
	:	STR_CONST
	|	FLT_CONST
	|	INT_CONST
;

%%

/* ADDITIONAL C CODE */

int main(){
  yyparse();
  return 0;
}