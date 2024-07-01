%{
    #include "pic.cc"
    extern "C" void yyerror(const char *s);
    extern int yylex(void);
	bool repl = false;
%}

%union{
	string *name;
	Statement *st;
	Expression *exp;
	Program *prog;
	StmtWrapper *stmt_wrap;
}

%token LET RETURN TRUE_VAL FALSE_VAL IF ELSE EQ NE LE GE AND OR INT_VAL FLT_VAL IDF STR_VAL

%left OR
%left AND
%right '!'
%nonassoc LE GE '<' '>' EQ NE
%left '+' '-'
%left '*' '/'
%right Uminus
%right '%'
%nonassoc IFX
%nonassoc ELSE

%type <prog> program
%type <stmt_wrap> stmt_list
%type <st> stmt let_stmt ret_stmt if_stmt exp_stmt
%type <exp> exp prefix_exp infix_exp
%type <name> INT_VAL FLT_VAL IDF STR_VAL

%start program
%%

program
	: stmt_list						{ $$ = new Program($1); }
;

stmt_list
	: stmt_list stmt				{ $1->push_back($2); $$ = $1; }
	| stmt							{ $$ = new StmtWrapper($1); }
;

stmt
	: let_stmt						{ $$ = $1; }
	| ret_stmt						{ $$ = $1; }
	| if_stmt						{ $$ = $1; }
	| exp_stmt						{ $$ = $1; }
;

let_stmt
	: LET IDF '=' exp ';'			{ $$ = new LetStmt(*($2), $4); }
;

ret_stmt
	: RETURN exp ';'				{ $$ = new RetStmt($2); }
;

if_stmt
	: IF '(' exp ')' stmt ELSE stmt	{ $$ = new IfStmt($3, $5, $7); }
	| IF '(' exp ')' stmt %prec IFX	{ $$ = new IfStmt($3, $5, NULL); }
;

exp_stmt
	: exp ';'						{ $$ = new ExpStmt($1); }
;

exp
	: prefix_exp					{ $$ = $1; }
	| infix_exp						{ $$ = $1; }
	| '(' exp ')'					{ $$ = $2; }
	| IDF							{ $$ = new Idf(*($1)); }
	| INT_VAL						{ $$ = new Const(*($1), INT_CST); }
	| FLT_VAL						{ $$ = new Const(*($1), FLT_CST); }
	| STR_VAL						{ $$ = new Const(*($1), STR_CST); }
	| TRUE_VAL						{ $$ = new Const(true); }
	| FALSE_VAL						{ $$ = new Const(false); }
;

prefix_exp
	: '-' exp %prec Uminus			{ $$ = new PrefixExp(NEG_PROP, $2); }
	| '!' exp						{ $$ = new PrefixExp(NOT_PROP, $2); }
;

infix_exp
	: exp '+' exp					{ $$ = new InfixExp($1, ADD_INOP, $3); }
	| exp '-' exp					{ $$ = new InfixExp($1, SUB_INOP, $3); }
	| exp '*' exp					{ $$ = new InfixExp($1, MUL_INOP, $3); }
	| exp '/' exp					{ $$ = new InfixExp($1, DIV_INOP, $3); }
	| exp '%' exp					{ $$ = new InfixExp($1, MOD_INOP, $3); }
	| exp EQ exp					{ $$ = new InfixExp($1, EQ_INOP, $3); }
	| exp NE exp					{ $$ = new InfixExp($1, NE_INOP, $3); }
	| exp '<' exp					{ $$ = new InfixExp($1, LT_INOP, $3); }
	| exp LE exp					{ $$ = new InfixExp($1, LE_INOP, $3); }
	| exp '>' exp					{ $$ = new InfixExp($1, GT_INOP, $3); }
	| exp GE exp					{ $$ = new InfixExp($1, GE_INOP, $3); }
	| exp AND exp					{ $$ = new InfixExp($1, AND_INOP, $3); }
	| exp OR exp					{ $$ = new InfixExp($1, OR_INOP, $3); }
;

%%

static int parse_opt (int key, char *arg, struct argp_state *state);

int main (int argc, char **argv) {
	struct argp_option options[] = {
		{ 0, 'i', 0, 0, "Interpret the input and print result"},
		{ 0 }
	};

	struct argp argp = { options, parse_opt };
	argp_parse (&argp, argc, argv, 0, 0, 0);
	return yyparse();
}


static int parse_opt (int key, char *arg, struct argp_state *state) {
	if (key == 'i') repl = true;
	return 0;
}