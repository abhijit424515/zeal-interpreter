%{
    #include<stdio.h>
%}

%option noyywrap
digit [0-9]
operator [-+*/]

%%
{digit}+ { printf("NUM: %s\n", yytext); }
{operator} { printf("OPERATOR: %c\n", yytext[0]); }

%%
int main() {
    yylex();
    return 0;
}