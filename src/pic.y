%{
	#include <cstdio>

	extern "C" void yyerror(char *s);
	extern int yylex(void);

	int decl_count = 0;
%}

%token READ WRITE ASSIGN COMMA LRB RRB LCB RCB PLUS MINUS MULT DIV TAB NEWLINE STR_CONST FLT_CONST INT_CONST NAME COMMENT

%%

program
	:	stmt_list
;

stmt_list
	:	stmt_list stmt
	|	%empty
;

stmt
	:	decl
	| NEWLINE
;

decl
	:	NAME ASSIGN constant NEWLINE			{ printf("DECL %d\n", ++decl_count); }
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