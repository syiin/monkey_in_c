#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "evaluator.h"
#include "environment.h"
#include "string.h"

void repl_start(FILE *in, FILE *out){
	char input[1024] = { '\0' };

	environment_t *env = new_environment();
	while(true){
		printf(">> ");
		if (fgets(input, sizeof(input), in) == NULL){
			return;
		}
		if (input[0] == '\n' || input[0] == '\0') {
			continue;
		}

		lexer_t *lexer = new_lexer(input);
		parser_t *parser = new_parser(lexer);
		program_t *program = parse_program(parser);

		if (parser->errors->count > 0){
			print_errors(parser);
			continue;
		}
	
		char buff_out[BUFSIZ] = {'\0'};
		object_t evaluated = eval(program, NODE_PROGRAM, env);
		inspect_object(evaluated, buff_out);
		printf("%s", buff_out);
	}
	
	/* TODO: Free allocated memory*/
}
