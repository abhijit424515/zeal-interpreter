%{
#include "common-headers.hh"
#include "code.hh"
#include "parse.tab.h"

%}
%option noyywrap

digit  [0-9]
ws	[ \t\n]*
operator [-+*/=;]
letter [a-zA-Z_]

%%
{digit}+ 	{ 	yylval.name= new string(yytext); return NUM; }

{operator}	{	return yytext[0];	}

{letter}({letter}|{digit})* { yylval.name= new string(yytext); return ID; }
		
{ws}		;
.	{ 	
		string error_message;
		error_message =  "Illegal character `" + *yytext;
		error_message += "' on line " + yylineno;
	}

