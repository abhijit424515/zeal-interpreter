%{
    #include<stdio.h>
%}

%option noyywrap

%%
dz { printf("FOUND: dz\n"); }
cdz { printf("FOUND: cdz\n"); }
bcdz { printf("FOUND: bcdz\n"); }
abcdz { printf("FOUND: abcdz\n"); }
. { printf("FOUND: unmatched\n"); }

%%
int main() {
    yylex();
    return 0;
}