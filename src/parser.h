#ifndef PARSER_H
#define PARSER_H

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

typedef enum {
	PRECEDENCE_LOWEST,
	PRECEDENCE_EQUALS ,// ==
	PRECEDENCE_NOT_EQUALS,
	PRECEDENCE_LESSGREATER, // > or <
	PRECEDENCE_SUM, // +
	PRECEDENCE_PRODUCT, // *
	PRECEDENCE_PREFIX ,// -X or !X
	PRECEDENCE_CALL, // myFunction(X)
	PRECEDENCE_INDEX, // myArray[1]
} precedence_t;

void parser_next_token(parser_t *parser);
program_t *new_program();
program_t *parse_program(parser_t *parser);
parser_t *new_parser(lexer_t *lexer);
program_t *push_to_program(statement_t *statement, program_t *program);
statement_t *parse_let_statement(parser_t *parser);
statement_t *parse_return_statement(parser_t *parser);
statement_t *parse_expression_statement(parser_t *parser);
statement_t *parse_statement(parser_t *parser);
expression_t *parse_if_expression(parser_t *parser);
expression_t *parse_function_literal(parser_t *parser);
block_statement_t *parse_block_statement(parser_t *parser);
vector_t *parse_function_parameters(parser_t *parser);

typedef expression_t *(*prefix_parser)(parser_t*);
typedef expression_t *(*infix_parser)(parser_t *, expression_t*);
prefix_parser parse_prefix_fns(TokenType token_type);
infix_parser parser_infix_fns(TokenType token_type);
expression_t *parse_expression(parser_t *parser, precedence_t precedence);
expression_t *parse_identifier(parser_t *parser);
expression_t *parse_integer_literal(parser_t *parser);
expression_t *parse_boolean(parser_t *parser);
expression_t *parse_array_literal(parser_t *parser);
expression_t *parse_hash_literal(parser_t *parser);
expression_t *parse_prefix_expression(parser_t *parser);
expression_t *parse_group_expression(parser_t *parser);
expression_t *parse_infix_expression(parser_t *parser, expression_t *left);
expression_t *parse_call_expression(parser_t *parser, expression_t *left);
expression_t *parse_index_expression(parser_t *parser, expression_t *left);
vector_t *parse_expression_list(parser_t *parser, TokenType end);
expression_t *parse_string_literal(parser_t*parser);
precedence_t get_precedence(TokenType token_type);
precedence_t curr_precedence(parser_t *parser);
precedence_t peek_precedence(parser_t *parser);

bool expect_peek(parser_t *parser, TokenType token_type);
bool peek_token_is(parser_t *parser, TokenType token_type);
bool curr_token_is(parser_t *parser, TokenType token_type);
void append_error(parser_t *parser, const char *error);
void peek_error(parser_t *parser, TokenType token_type);
void print_errors(parser_t *parser);
bool expressions_equal(expression_t *a, expression_t *b);

#endif
