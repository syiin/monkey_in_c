#include "ast.h"
#include "token.h"
#include "lexer.h"
#include <stdlib.h>

typedef struct Parser {
	lexer_t *lexer;

	token_t	curr_token;
	token_t	peek_token;
} parser_t;


void parser_next_token(parser_t *parser);
program_t *parse_program(parser_t *parser);
parser_t *new_parser(lexer_t *lexer);
