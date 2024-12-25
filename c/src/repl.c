#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"


void repl_start(FILE *in, FILE *out){
	char input[1024] = { '\0' };
	while(true){
		printf(">> ");
		if (fgets(input, sizeof(input), in) == NULL){
			return;
		}

		lexer_t *lexer = new_lexer(input);
		parser_t *parser = new_parser(lexer);
		program_t *program = parse_program(parser);

		if (parser->errors->count > 0){
			print_errors(parser);
			continue;
		}

		printf(
			"%s\n",
			program_to_string(program)
		);
	}
	
	/* TODO: Free allocated memory*/
}
