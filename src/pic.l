/* DEFINITIONS */

/* yywrap handles the end-of-input logic, if noyywrap is used, program exits on end-of-file (EOF) */
%option noyywrap 

digit                       [0-9]
frac                        (([0-9]*\.[0-9]+)|([0-9]+\.[0-9]*))
ws	                        [ \t\n]*
letter                      [a-zA-Z_]
comment										  (#.*)

/* RULES */
%%

read			 			{ printf("READ\n"); }
write			 			{ printf("WRITE\n"); }

=         			{ printf("ASSIGN\n"); }
,         			{ printf("COMMA\n"); }
\(        			{ printf("LRB\n"); }
\)        			{ printf("RRB\n"); }
\{        			{ printf("LCB\n"); }
\}        			{ printf("RCB\n"); }
"+"       			{ printf("PLUS\n"); }
"-"       			{ printf("MINUS\n"); }
"*"       			{ printf("MULT\n"); }
"/"       			{ printf("DIV\n"); }

"\t"      			{ printf("tab\n"); }
"\n"      			{ printf("newline\n"); }
" "       			;

\"([^\"]*)\"								{ printf("STR_CONST\n"); }
{frac}											{ printf("FLT_CONST\n"); }
{digit}+										{ printf("INT_CONST\n"); }
{letter}({letter}|{digit})* { printf("NAME\n"); }
{comment}										{ printf("COMMENT\n"); }

.         			{ fprintf(stderr, "unknown_token: %d\n", yytext[0]); exit(1); }

%%

/* ADDITIONAL C CODE */

int main(){
  yylex();
  return 0;
}
