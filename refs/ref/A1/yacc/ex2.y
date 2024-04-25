%{
#include <stdio.h>
#include <stdlib.h>
%}

%token NUM

%%
E   : NUM { printf("EXPRESSION (NUMBER)\n"); }
    | E '+' E { $$ = $1+$3; printf("EXPRESSION (PLUS): %d\n", $$); }

%%
int yyerror(char *mesg) {
    fprintf(stderr, "%s\n", mesg);
    exit(1);
}