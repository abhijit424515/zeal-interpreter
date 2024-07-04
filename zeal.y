%{
    #include "zeal.cc"
    extern "C" void yyerror(const char *s);
    extern int yylex(void);
	bool repl = true;
%}

%union{
	string *name;
	Statement *st;
	Expression *exp;
	Program *prog;
	StmtWrapper *stmt_wrap;
	BlockStmt *block;
	Call *call;
	ArgWrapper *arg;
	ExpWrapper *exp_wrap;
}

%token LET RETURN TRUE_VAL FALSE_VAL IF ELSE EQ NE LE GE AND OR INT_VAL FLT_VAL IDF STR_VAL NULL_VAL FN

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
%type <st> stmt let_stmt asg_stmt ret_stmt if_stmt exp_stmt
%type <exp> exp prefix_exp infix_exp fn
%type <name> INT_VAL FLT_VAL IDF STR_VAL
%type <block> block_stmt
%type <arg> arg_list
%type <call> call
%type <exp_wrap> exp_list

%start program
%%

program
	: stmt_list							{ $$ = new Program($1); $$->code(); }
	| %empty							{ $$ = new Program(); }
;

stmt_list
	: stmt_list stmt					{ $1->push_back($2); $$ = $1; }
	| stmt								{ $$ = new StmtWrapper($1); }
;

block_stmt
	: '{' stmt_list '}'					{ $$ = new BlockStmt($2); }
	| '{' '}'							{ $$ = new BlockStmt(new StmtWrapper()); }
;

fn
	: FN '(' arg_list ')' block_stmt	{ $$ = new Const($3, $5); }
	| FN '(' ')' block_stmt				{ $$ = new Const(new ArgWrapper(), $4); }
;

call
	: IDF '(' exp_list ')'				{ $$ = new Call(*($1), $3); }
	| IDF '(' ')'						{ $$ = new Call(*($1), new ExpWrapper()); }
;

exp_list
	: exp_list ',' exp					{ $1->push_back($3); $$ = $1; }
	| exp								{ $$ = new ExpWrapper($1); }
;

arg_list
	: arg_list ',' IDF					{ $1->push_back($3); $$ = $1; }
	| IDF								{ $$ = new ArgWrapper($1); }
;

stmt
	: let_stmt							{ $$ = $1; }
	| asg_stmt							{ $$ = $1; }
	| ret_stmt							{ $$ = $1; }
	| if_stmt							{ $$ = $1; }
	| exp_stmt							{ $$ = $1; }
;

let_stmt
	: LET IDF '=' exp ';'				{ $$ = new LetStmt(*($2), $4); }
;

asg_stmt
	: IDF '=' exp ';'					{ $$ = new AsgStmt(*($1), $3); }
;

ret_stmt
	: RETURN exp ';'					{ $$ = new RetStmt($2); }
;

if_stmt
	: IF '(' exp ')' block_stmt ELSE block_stmt		{ $$ = new IfStmt($3, $5, $7); }
	| IF '(' exp ')' block_stmt %prec IFX			{ $$ = new IfStmt($3, $5, NULL); }
;

exp_stmt
	: exp ';'							{ $$ = new ExpStmt($1); }
;

exp
	: prefix_exp						{ $$ = $1; }
	| infix_exp							{ $$ = $1; }
	| '(' exp ')'						{ $$ = $2; }
	| fn								{ $$ = $1; }
	| call								{ $$ = $1; }
	| IDF								{ $$ = new Idf(*($1)); }
	| INT_VAL							{ $$ = new Const(*($1), ConstType::INT); }
	| FLT_VAL							{ $$ = new Const(*($1), ConstType::FLT); }
	| STR_VAL							{ $$ = new Const(*($1), ConstType::STR); }
	| TRUE_VAL							{ $$ = new Const(true); }
	| FALSE_VAL							{ $$ = new Const(false); }
	| NULL_VAL							{ $$ = new Const(); }
;

prefix_exp
	: '-' exp %prec Uminus				{ $$ = new PrefixExp(PrefixOp::NEG, $2); }
	| '!' exp							{ $$ = new PrefixExp(PrefixOp::NOT, $2); }
;

infix_exp
	: exp '+' exp						{ $$ = new InfixExp($1, InfixOp::ADD, $3); }
	| exp '-' exp						{ $$ = new InfixExp($1, InfixOp::SUB, $3); }
	| exp '*' exp						{ $$ = new InfixExp($1, InfixOp::MUL, $3); }
	| exp '/' exp						{ $$ = new InfixExp($1, InfixOp::DIV, $3); }
	| exp '%' exp						{ $$ = new InfixExp($1, InfixOp::MOD, $3); }
	| exp EQ exp						{ $$ = new InfixExp($1, InfixOp::EQ, $3); }
	| exp NE exp						{ $$ = new InfixExp($1, InfixOp::NE, $3); }
	| exp '<' exp						{ $$ = new InfixExp($1, InfixOp::LT, $3); }
	| exp LE exp						{ $$ = new InfixExp($1, InfixOp::LE, $3); }
	| exp '>' exp						{ $$ = new InfixExp($1, InfixOp::GT, $3); }
	| exp GE exp						{ $$ = new InfixExp($1, InfixOp::GE, $3); }
	| exp AND exp						{ $$ = new InfixExp($1, InfixOp::AND, $3); }
	| exp OR exp						{ $$ = new InfixExp($1, InfixOp::OR, $3); }
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
	if (key == 'i') repl = true; // [TODO] add more options
	return 0;
}