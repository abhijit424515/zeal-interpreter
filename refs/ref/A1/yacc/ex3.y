%{
#include <stdio.h>
#include <stdlib.h>
%}

%token NUM
%left '+' '-'
%left '*' '/'

%%
E   : NUM { printf("EXPRESSION (NUMBER)\n"); }
    | E '+' E { $$ = $1+$3; printf("EXPRESSION (PLUS): %d\n", $$); }
    | E '-' E { $$ = $1-$3; printf("EXPRESSION (MINUS): %d\n", $$); }
    | E '*' E { $$ = $1*$3; printf("EXPRESSION (MULT): %d\n", $$); }
    | E '/' E { $$ = $1/$3; printf("EXPRESSION (DIV): %d\n", $$); }

%%
int yyerror(char *mesg) {
    fprintf(stderr, "%s\n", mesg);
    exit(1);
}