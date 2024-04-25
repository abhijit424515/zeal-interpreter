%{
# include <stdio.h>
# include <iostream>
# include <string.h>
extern "C" void yyerror(char *s);

extern int yylex(void);

using namespace std;

%}

%token NUM
%left '+' '-'
%left '*' '/'
%right UMINUS
%start Start
%%

Start : Expr 
	{
		cout << "The result is " << $1 << endl;
	}
	;

Expr : Expr '+' Expr			
		{	$$ = $1 + $3; }
	| Expr '*' Expr 		
		{	$$ = $1 * $3; }
	| Expr '/' Expr 		
		{	$$ = $1 / $3; }
	| Expr '-' Expr 		
		{	$$ = $1 - $3; }
	| '-' Expr	%prec UMINUS	
		{	$$ = - $2; 
		}
	| NUM
	;
	
%%

#if 0
int main()
{
	yyparse();
}
#endif
