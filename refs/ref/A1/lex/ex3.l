%{
    #include<stdio.h>
%}

%option noyywrap
digit [0-9]
letter [a-zA-Z_]

%%
int { printf("KEYWORD: %s\n", yytext); }
float { printf("KEYWORD: %s\n", yytext); }
{letter}({letter}|{digit})* { printf("ID: %s\n", yytext); }
{digit}+ { printf("NUM: %s\n", yytext); }
. {}
\n {}

%%
int main() {
    yylex();
    return 0;
}