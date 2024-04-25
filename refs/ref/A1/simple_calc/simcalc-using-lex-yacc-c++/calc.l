%{
#include<string>
#include<iostream>

void store_token_name(std::string token_name, char *lexeme,int lineno);

using namespace std;

#include "parse.tab.h"

%}
%option noyywrap

digit  [0-9]
ws	[ \t\n]*
operator [-+*/]

%%
{digit}+ 	{ 	yylval = atoi(yytext);
			store_token_name("NUM",yytext,yylineno);
		       	return NUM; 
		}
{operator}	{	
			store_token_name("OPERATOR",yytext,yylineno);
			return yytext[0];	
		}
{ws}		;
'\n'		{	
			yylineno++;
		}
.	{ 	
		string error_message;
		error_message =  "Illegal character `" + *yytext;
		error_message += "' on line " + yylineno;
	}
%%

