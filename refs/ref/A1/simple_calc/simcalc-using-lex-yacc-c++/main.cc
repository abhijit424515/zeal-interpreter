# include <stdio.h>
# include <argp.h>
# include <string>
# include <iostream>

using namespace std;

static int parse_opt (int key, char *arg, struct argp_state *state);
int yyparse();
int yylex();

bool show_tokens = false;
bool stop_after_scanning = false;	

int main (int argc, char **argv)
{
	struct argp_option options[] = {
  		{ "show-tokens", 't' , 0, 0, "Show the tokens in FILE.toks (or out.toks)", 9 },
  		{ "sa-scan",  's' , 0, 0, "Stop after lexical analysis", 0 },
		{ 0 }
		};

	struct argp argp = {options, parse_opt };

	argp_parse (&argp, argc, argv, 0, 0, 0);
	if (stop_after_scanning)
	{
		while (yylex());
	}
	else
		yyparse();
	return 0;
}

static int parse_opt (int key, char *arg, struct argp_state *state)
{
	switch (key)
	{
		case 't': 
			show_tokens = true;	
			break;
		case 's': 
			stop_after_scanning = true;	
			break;
	}
	return 0;
}

void store_token_name(string token_name, char *lexeme, int lineno)
{
	if (show_tokens)
  	{
    		cout << "\tToken Name: " << token_name;
    		cout << " \tLexeme: " << lexeme;
    		cout <<" \t Lineno: "<< lineno <<"\n";
  	}
}

