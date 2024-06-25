%{
    #include "pic.cc"
    extern "C" void yyerror(const char *s);
    extern int yylex(void);
%}

%union{
	string *name;
}

%token LET RETURN TRUE_VAL FALSE_VAL IF ELSE LT GT EQ LE GE NE AND OR NOT INT_VAL FLT_VAL IDF STR_VAL

%left '+' '-'
%left '*' '/'
%right Uminus

%type <name> INT_VAL FLT_VAL IDF STR_VAL

%start program
%%

program
	: stmt_list
;

stmt_list
	: stmt_list stmt
	| stmt
;

func
	: IDF '(' param_list ')' compound_stmt ';'
;

%%

int main() {
	yyparse();
	return 0;
}