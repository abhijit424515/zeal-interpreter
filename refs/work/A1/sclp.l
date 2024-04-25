%{
#include<string>
#include<iostream>
#include "y.tab.h"
using namespace std;

void store_token_name(std::string token_name, char *lexeme,int lineno);
%}

%option noyywrap

digit                       [0-9]
fraction                    ([0-9]+\.[0-9]*) 
fraction2                   ([0-9]*\.[0-9]+) 
ws	                        [ \t\n]*
letter                      [a-zA-Z_]
inline                      "//"(.*)

%%
{inline}                    ;
{ws}                        ;
int                         { store_token_name("INTEGER",yytext,yylineno); return INTEGER; }
float                       { store_token_name("FLOAT",yytext,yylineno); return FLOAT; }
void                        { store_token_name("VOID",yytext,yylineno); return VOID; }
string                      { store_token_name("STRING",yytext,yylineno); return STRING; }
bool                        { store_token_name("BOOL",yytext,yylineno); return BOOL; }
read                        { store_token_name("READ",yytext,yylineno); return READ; }
print                       { store_token_name("WRITE",yytext,yylineno); return WRITE; }

=                           { store_token_name("ASSIGN_OP",yytext,yylineno); return ASSIGN; }
;                           { store_token_name("SEMICOLON",yytext,yylineno); return yytext[0]; }
,                           { store_token_name("COMMA",yytext,yylineno); return yytext[0]; }
\(                          { store_token_name("LEFT_ROUND_BRACKET",yytext,yylineno); return yytext[0]; }
\)                          { store_token_name("RIGHT_ROUND_BRACKET",yytext,yylineno); return yytext[0]; }
\{                          { store_token_name("LEFT_CURLY_BRACKET",yytext,yylineno); return yytext[0]; }
\}                          { store_token_name("RIGHT_CURLY_BRACKET",yytext,yylineno); return yytext[0]; }
"+"                         { store_token_name("PLUS",yytext,yylineno); return yytext[0]; }
"-"                         { store_token_name("MINUS",yytext,yylineno); return yytext[0]; }
"*"                         { store_token_name("MULT",yytext,yylineno); return yytext[0]; }
"/"                         { store_token_name("DIV",yytext,yylineno); return yytext[0]; }

\"([^\"]*)\"                 { store_token_name("STR_CONST",yytext,yylineno); return STRING_CONSTANT; }
{fraction}                 { store_token_name("FLOAT_NUM",yytext,yylineno); yylval = atof(yytext); return DOUBLE_NUMBER; }
{fraction2}                 { store_token_name("FLOAT_NUM",yytext,yylineno); yylval = atof(yytext); return DOUBLE_NUMBER; }
{digit}+                    { store_token_name("INT_NUM",yytext,yylineno); yylval = atoi(yytext); return INTEGER_NUMBER; }
{letter}({letter}|{digit})* { store_token_name("NAME",yytext,yylineno); return NAME; }

.                           { fprintf(stderr, "Illegal character \"%c\" on line number %d\n", yytext[0], yylineno); exit(1); }

%%