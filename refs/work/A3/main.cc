# include <iostream>
# include <string.h>
# include <stdio.h>
# include <argp.h>
# include <unistd.h>
# include <fcntl.h>
# include "common-headers.hh"

using namespace std;
extern FILE* yyin;

static int parse_opt (int key, char *arg, struct argp_state *state);
int yyparse();
int yylex();

static char doc[] = "Sclp - A language processor for C-like language";
bool show_tokens = false;
bool show_ast = false;
bool show_tac = false;
bool show_rtl = false;
bool show_asm = false;
bool stop_after_scan = false;
bool stop_after_parse = false;
bool stop_after_ast = false;
bool stop_after_tac = false;
bool stop_after_rtl = false;
bool demo_mode = false;
char *filename = NULL;

int main (int argc, char **argv) {
	struct argp_option options[] = {
  		{ "show-tokens", 't' , 0, 0, "Show the tokens in FILE.toks (or out.toks)", 1 },
		{ "show-ast", 'a' , 0, 0, "Show abstract syntax trees in FILE.ast (or out.ast)", 1 },
		{ "show-tac", 'c' , 0, 0, "Show the Three Address Code in FILE.tac (or out.tac)", 1 },
		{ "show-rtl", 'r' , 0, 0, "Show the Register Transfer Language code in FILE.rtl (or out.rtl)", 1 },
		{ "show-asm", 'm' , 0, 0, "Generate the assembly program in FILE.spim (or out.spim). This is the default action and is suppressed only if a valid `sa-...' option is given to stop the compilation after some earlier phase.", 1 },
  		{ "sa-scan",  'S' , 0, 0, "Stop after scanning", 2 },
		{ "sa-parse",  'P' , 0, 0, "Stop after parsing", 2 },
		{ "sa-ast",  'A' , 0, 0, "Stop after constructing Abstract Syntax Tree (AST)", 2 },
		{ "sa-tac",  'C' , 0, 0, "Stop after constructing Three Address Code (TAC)", 2 },
		{ "sa-rtl",  'R' , 0, 0, "Stop after constructing Register Transfer Language (RTL) code", 2 },
		{ "demo",  'd' , 0, 0, "Demo version. Use stdout for the output instead of files", 0 },
		{ "version",  'V' , 0, 0, "Print program version", 0 },
		{ 0 }
    };
	struct argp argp = { options, parse_opt, "[FILE]", doc, 0, 0, 0};
	argp_parse (&argp, argc, argv, 0, 0, 0);
	if (!filename) {
		cerr << "No filename provided" << endl;
		return 1;
	}
	yyin = fopen(filename, "r");
	if (stop_after_scan) while (yylex());
	else return yyparse();
	return 0;
}

static int parse_opt (int key, char *arg, struct argp_state *state) {
	switch (key) {
		case 't': 
			show_tokens = true;	
			break;
		case 'a':
			show_ast = true;	
			break;
		case 'c':
			show_tac = true;
			break;
		case 'r':
			cerr << "sclp error: File:  The RTL related options are not supported. The current implementation supports only SCAN and PARSE.\n";
			exit(1);
			break;
		case 'm':
			cerr << "sclp error: File:  The RTL related options are not supported. The current implementation supports only SCAN and PARSE.\n";
			exit(1);
			break;
		case 'S': 
			stop_after_scan = true;	
			break;
		case 'P': 
			stop_after_parse = true;	
			break;
		case 'A': 
			stop_after_ast = true;	
			break;
		case 'C': 
			stop_after_tac = true;	
			break;
		case 'R': 
			stop_after_rtl = true;	
			break;
		case 'd': 
			demo_mode = true;	
			break;
		case 'V':
			cout << "Sclp Version: A3\n";
			exit(0);
		case ARGP_KEY_ARG:
			if (state->arg_num == 0) {
				filename = arg;
			} else {
				argp_usage(state);
			}
			break;
	}
	return 0;
}

bool first_stn = true;

void store_token_name(string token_name, char *lexeme, int lineno) {
	size_t len_x = strlen(filename);
	size_t len_y = len_x + strlen(".toks") + 1;
	char *new_filename = (char*) malloc(len_y*sizeof(char));
	strcpy(new_filename, filename);
	strcat(new_filename, ".toks");

	if (show_tokens) {
		if (first_stn && !demo_mode) {
			if (access(new_filename, F_OK) != -1)
				unlink(new_filename);
			first_stn = false;
		}
		
		int fd;
		int stdout_orig;
		
		if (!demo_mode) {
			if ((fd = open(new_filename, O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1) {
				cerr << "Error creating .toks file\n";
				exit(1);
			}
			stdout_orig = dup(STDOUT_FILENO);
			if (dup2(fd, STDOUT_FILENO) == -1) {
				cerr << "Error redirecting output to .toks file\n";
				close(fd);
				exit(1);
			}
		} 
		
		cout << "\tToken Name: " << token_name;
		cout << " \tLexeme: " << lexeme;
		cout <<" \t Lineno: "<< lineno << "\n";
		
		cout << flush;
		if (!demo_mode) {
			dup2(stdout_orig, STDOUT_FILENO);
			close(fd);
		}
  	}
}

