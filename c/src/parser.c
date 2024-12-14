#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "vector.h"

parser_t *new_parser(lexer_t *lexer){
	parser_t *parser = malloc(sizeof(parser_t));
	if (parser == NULL){
		return NULL;
	}
	parser->lexer = lexer;
	parser->errors = create_vector();
	parser_next_token(parser);
	return parser;
}

void parser_next_token(parser_t *parser){
	copy_token(&parser->curr_token, &parser->peek_token);
	parser->peek_token = *lexer_next_token(parser->lexer);
}

statement_t *parse_statement(parser_t *parser){
	switch(parser->curr_token.type){
		case LET:
			return parse_let_statement(parser);
		case RETURN:
			return parse_return_statement(parser);
		default:
			return parse_expression_statement(parser);
	}
}

statement_t *parse_return_statement(parser_t *parser){
	statement_t *statement = malloc(sizeof(statement_t));
	if (statement == NULL){
		return NULL;
	}

	statement->type = RETURN_STATEMENT;
	statement->token = parser->curr_token;

	parser_next_token(parser);

	if (!(curr_token_is(parser, SEMICOLON))){
		parser_next_token(parser);
	}

	return statement;
}

statement_t *parse_let_statement(parser_t *parser){
	statement_t *statement = malloc(sizeof(statement_t));
	if (statement == NULL){
		return NULL;
	}

	statement->type = LET_STATEMENT;
	statement->token = parser->curr_token;
	
	if (!(expect_peek(parser, IDENT))){
		return NULL;
	}

	statement->name.token = parser->curr_token;
	statement->name.value = parser->curr_token.literal;

	if (!(expect_peek(parser, ASSIGN))){
		return NULL;
	}

	if (!(curr_token_is(parser, SEMICOLON))){
		parser_next_token(parser);
	}

	return statement;
}

expression_t *parse_expression(parser_t *parser, Precedence precedence){
	parse_prefix_expression prefix_fn = parse_prefix_fns(parser->curr_token.type);
	return prefix_fn(parser);
}

statement_t *parse_expression_statement(parser_t *parser){
	statement_t *statement = malloc(sizeof(statement_t));
	if (statement == NULL){
		return NULL;
	}

	statement->type = EXPRESSION_STATEMENT;
	statement->value = parse_expression(parser, LOWEST);
	
	if (!(expect_peek(parser, SEMICOLON))){
		return NULL;
	}
	return statement;
}

bool expect_peek(parser_t *parser, TokenType token_type){
	if (peek_token_is(parser, token_type)){
		parser_next_token(parser);
		return true;
	} else {
		peek_error(parser, token_type);
		return false;
	}
}

bool peek_token_is(parser_t *parser, TokenType token_type){
	return parser->peek_token.type == token_type;
}

bool curr_token_is(parser_t *parser, TokenType token_type){
	return parser->curr_token.type == token_type;
}

void peek_error(parser_t *parser, TokenType token_type){
	const char *received = token_type_to_string(parser->peek_token.type);
	const char *expected = token_type_to_string(token_type);
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "expected next token to be %s, got %s instead", expected, received);
	append_error(parser, buffer);
}

void append_error(parser_t *parser, const char *error){
	append_vector(parser->errors, strdup(error));
}

void print_errors(parser_t *parser){
	vector_t *errors = parser->errors;
	for (int i = 0; i < errors->count; i++){
		printf("%s\n", (char *)errors->data[i]);
	}
}

program_t *parse_program(parser_t *parser){
	int initial_cap = 2;
	program_t *program = malloc(sizeof(program_t) + sizeof(statement_t) * initial_cap);
	program->statement_cap = initial_cap;
	program->count = 0;

	while (parser->curr_token.type != EOF_TOKEN){
		statement_t *statement = parse_statement(parser);
		if (statement != NULL){
			program = push_to_program(statement, program);
		}
		parser_next_token(parser);
	}
	return program;
}

program_t *push_to_program(statement_t *statement, program_t *program){
	if (program->count >= program->statement_cap){
		program->statement_cap = program->statement_cap * 2;
		size_t to_allocate = sizeof(program_t) + program->statement_cap * sizeof(statement_t);
		program_t *new_program = realloc(program, to_allocate);
		assert(new_program != NULL);
		program = new_program;
	}
	program->statements[program->count] = statement;
	program->count++;
	return program;
}

parse_prefix_expression parse_prefix_fns(TokenType token_type){
	switch(token_type){
		case IDENT:
			return parse_identifier;
		default:
			return NULL;
	}
}

expression_t *parse_identifier(parser_t *parser){
	token_t token = {
			.type = IDENT,
			.literal = "anotherVar"
		};
	return new_expression(IDENT_EXPR, token);
};

