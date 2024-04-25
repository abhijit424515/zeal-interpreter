%{
# include "common-headers.hh"
# include "parse.tab.h"

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

"!"                         { store_token_name("NOT",yytext,yylineno); return NOT; }
"=="                        { store_token_name("EQUAL",yytext,yylineno); return EQ; }
"!="                        { store_token_name("NOT_EQUAL",yytext,yylineno); return NE; }
">"                         { store_token_name("GREATER_THAN",yytext,yylineno); return GT; }
"<"                         { store_token_name("LESS_THAN",yytext,yylineno); return LT; }
"&&"                        { store_token_name("AND",yytext,yylineno); return AND; }
"?"                         { store_token_name("QUESTION_MARK",yytext,yylineno); return yytext[0]; }
":"                         { store_token_name("COLON",yytext,yylineno); return yytext[0]; }
">="                        { store_token_name("GREATER_THAN_EQUAL",yytext,yylineno); return GE; }
"<="                        { store_token_name("LESS_THAN_EQUAL",yytext,yylineno); return LE; }
"||"                        { store_token_name("OR",yytext,yylineno); return OR; }

\"([^\"]*)\"                { store_token_name("STR_CONST",yytext,yylineno); yylval.name = new string(yytext); return STRING_CONSTANT; }
{fraction}                  { store_token_name("FLOAT_NUM",yytext,yylineno); yylval.name = new string (yytext); return DOUBLE_NUMBER; }
{fraction2}                 { store_token_name("FLOAT_NUM",yytext,yylineno); yylval.name = new string(yytext); return DOUBLE_NUMBER; }
{digit}+                    { store_token_name("INT_NUM",yytext,yylineno); yylval.name = new string(yytext); return INTEGER_NUMBER; }

if                          { fprintf(stderr, "sclp error: File contains some feature of language level L4. The current implementation supports levels L1, L2, and L3.\n"); exit(1); }
else                        { fprintf(stderr, "sclp error: File contains some feature of language level L4. The current implementation supports levels L1, L2, and L3.\n"); exit(1); }
while                       { fprintf(stderr, "sclp error: File contains some feature of language level L4. The current implementation supports levels L1, L2, and L3.\n"); exit(1); }
do                          { fprintf(stderr, "sclp error: File contains some feature of language level L4. The current implementation supports levels L1, L2, and L3.\n"); exit(1); }
return                      { fprintf(stderr, "sclp error: File contains some feature of language level L5. The current implementation supports levels L1, L2, and L3.\n"); exit(1); }
"&"                         { fprintf(stderr, "sclp error: File contains some feature of language level L6. The current implementation supports levels L1, L2, and L3.\n"); exit(1); }

{letter}({letter}|{digit})* { store_token_name("NAME",yytext,yylineno); yylval.name = new string(yytext); return NAME; }

.                           { fprintf(stderr, "            Description: Illegal character \'%c\'\n", yytext[0]); exit(1); }

%%