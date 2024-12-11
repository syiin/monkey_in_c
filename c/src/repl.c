#include <stdio.h>
#include "lexer.h"
#include "token.h"

void repl_start(FILE *in, FILE *out){
	char input[1024] = { '\0' };
	while(true){
		printf(">> ");
		if (fgets(input, sizeof(input), in) == NULL){
			return;
		}

		lexer_t *lexer = new_lexer(input);
		token_t *token = lexer_next_token(lexer);
		while(token->type != EOF_TOKEN){
			print_token(token);
			token = lexer_next_token(lexer);
		}
	}
	
}
