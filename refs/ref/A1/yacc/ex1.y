%{
#include <stdio.h>
#include <stdlib.h>
%}

%token NUM

%%
E : NUM { printf("EXPRESSION (NUMBER)\n"); }

%%
int yyerror(char *mesg) {
    fprintf(stderr, "%s\n", mesg);
    exit(1);
}