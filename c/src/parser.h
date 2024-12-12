#include "ast.h"
#include "token.h"
#include "lexer.h"
#include "vector.h"
#include <stdlib.h>

typedef struct Parser {
	lexer_t *lexer;

	token_t	curr_token;
	token_t	peek_token;

	vector_t *errors;
} parser_t;

typedef struct ParserError {
	char *error;
} parser_error_t;

void parser_next_token(parser_t *parser);
program_t *parse_program(parser_t *parser);
parser_t *new_parser(lexer_t *lexer);
program_t *push_to_program(statement_t *statement, program_t *program);
statement_t *parse_let_statement(parser_t *parser);
statement_t *parse_statement(parser_t *parser);
bool expect_peek(parser_t *parser, TokenType token_type);
bool peek_token_is(parser_t *parser, TokenType token_type);
bool curr_token_is(parser_t *parser, TokenType token_type);
void append_error(parser_t *parser, const char *error);
void peek_error(parser_t *parser, TokenType token_type);
void print_errors(parser_t *parser);
