%{
	#include "zeal.hh"
	#include "parse.tab.h"
	using namespace std;
%}

%option noyywrap 

digit						[0-9]
fraction					([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)
ws							[ \t\n]*
letter						[a-zA-Z_]
comment						("//".*) 

%%

{comment}					;
let							{ return LET; }
return						{ return RETURN; }
true						{ return TRUE_VAL; }
false						{ return FALSE_VAL; }
if							{ return IF; }
else						{ return ELSE; }
null						{ return NULL_VAL; }
fn							{ return FN; }

=							{ return '='; }
;							{ return ';'; }
,							{ return ','; }
:							{ return ':'; }
\?							{ return '?'; }
\+							{ return '+'; }
\-							{ return '-'; }
\*							{ return '*'; }
\/							{ return '/'; }
\^							{ return '^'; }
\<							{ return '<'; }
\>							{ return '>'; }
==							{ return EQ; }
!=							{ return NE; }
\<=							{ return LE; }
\>=							{ return GE; }
&&							{ return AND; }
\|\|						{ return OR; }
\!							{ return '!'; }
\(							{ return '('; }
\)							{ return ')'; }
\{							{ return '{'; }
\}							{ return '}'; }
\[							{ return '['; }
\]							{ return ']'; }

{digit}+					{ yylval.name = new string(yytext); return INT_VAL; }
{fraction}					{ yylval.name = new string(yytext); return FLT_VAL; }
{letter}({letter}|{digit})*	{ yylval.name = new string(yytext); return IDF; }
\"[^\"\\]*\"				{ yylval.name = new string(yytext); return STR_VAL; /* ignoring escape chars for now */ } 

{ws}						;
.							{ fprintf(stderr, "unknown_token: %d\n", yytext[0]); exit(1); }
