%{
	#include "parse.tab.h"
	#include <string.h>
%}

/* DEFINITIONS */

/* yywrap handles the end-of-input logic, if noyywrap is used, program exits on end-of-file (EOF) */
%option noyywrap 

digit                       [0-9]
frac                        (([0-9]*\.[0-9]+)|([0-9]+\.[0-9]*))
indent	                    [\t]+
letter                      [a-zA-Z_]
comment										  (#.*)

/* RULES */
%%

read			 									{ return READ; }
write			 									{ return WRITE; }
loop												{ return LOOP; }

=         									{ return ASSIGN; }
,         									{ return COMMA; }
\(        									{ return LRB; }
\)        									{ return RRB; }
\{        									{ return LCB; }
\}        									{ return RCB; }
"+"       									{ return PLUS; }
"-"       									{ return MINUS; }
"*"       									{ return MULT; }
"/"       									{ return DIV; }

"\n"                        { return NEWLINE; }
{indent}										{ yylval = strlen(yytext); return INDENT; }
" "       									; /* Ignore spaces */

\"([^\"]*)\"								{ return STR_CONST; }
{frac}											{ return FLT_CONST; }
{digit}+										{ return INT_CONST; }
{letter}({letter}|{digit})* { return NAME; }
{comment}										;

.         									{ fprintf(stderr, "unknown_token: %d\n", yytext[0]); exit(1); }

%%