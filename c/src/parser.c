#include <stdio.h>
#include "parser.h"
#include "lexer.h"
#include "token.h"

parser_t *new_parser(lexer_t *lexer){
	parser_t *parser = malloc(sizeof(parser_t));
	if (parser == NULL){
		return NULL;
	}
	parser->lexer = lexer;

	parser_next_token(parser);
	parser_next_token(parser);

	return parser;
}

void parser_next_token(parser_t *parser){
	parser->curr_token = parser->peek_token;
	parser->peek_token = *lexer_next_token(parser->lexer);
}

program_t *parse_program(parser_t *parser){
	return NULL;
}
