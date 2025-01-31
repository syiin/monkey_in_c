#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "vector.h"
#include "object.h"

parser_t *new_parser(lexer_t *lexer){
	parser_t *parser = malloc(sizeof(parser_t));
	if (parser == NULL){
		return NULL;
	}
	parser->lexer = lexer;
	parser->errors = create_vector();
	parser->curr_token = *lexer_next_token(parser->lexer);
	parser->peek_token = *lexer_next_token(parser->lexer);
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
		case ILLEGAL:
			return NULL;
		case SEMICOLON:
			return NULL;
		default:
			return parse_expression_statement(parser);
	}
}

statement_t *parse_return_statement(parser_t *parser){
	statement_t * statement = new_statement(RETURN_STATEMENT);
	if (statement == NULL){
		return NULL;
	}

	token_t token = {
			.type = parser->curr_token.type,
			.literal = strdup(parser->curr_token.literal)
		};
	statement->token = token;

	parser_next_token(parser);

	if (!curr_token_is(parser, SEMICOLON)) {
		statement->value = parse_expression(parser, PRECEDENCE_LOWEST);
	}

	if (!(curr_token_is(parser, SEMICOLON))){
		parser_next_token(parser);
	}

	return statement;
}

statement_t *parse_let_statement(parser_t *parser){
	statement_t *statement = new_statement(LET_STATEMENT);
	if (statement == NULL){
		return NULL;
	}

	statement->token = parser->curr_token;

	if (!(expect_peek(parser, IDENT))){
		return NULL;
	}

	statement->name.token = parser->curr_token;
	statement->name.value = parser->curr_token.literal;

	if (!(expect_peek(parser, ASSIGN))){
		return NULL;
	}

	parser_next_token(parser);
	statement->value = parse_expression(parser, PRECEDENCE_LOWEST);

	if (!(curr_token_is(parser, SEMICOLON))){
		parser_next_token(parser);
	}

	return statement;
}

statement_t *parse_expression_statement(parser_t *parser){
	statement_t *statement = new_statement(EXPRESSION_STATEMENT);
	if (statement == NULL){
		return NULL;
	}

	statement->token = parser->curr_token;
	statement->value = parse_expression(parser, PRECEDENCE_LOWEST);

	if (peek_token_is(parser, SEMICOLON)) {
		parser_next_token(parser);
	}
	return statement;
}

expression_t *parse_expression(parser_t *parser, precedence_t precedence){
	prefix_parser prefix_fn = parse_prefix_fns(parser->curr_token.type);
	if (prefix_fn == NULL) {
		char error[100];
		snprintf(error, sizeof(error), "no prefix parse function for %s found",
			token_type_to_string(parser->curr_token.type));
		append_error(parser, error);
		return NULL;
	}

	expression_t *left = prefix_fn(parser);
	if (left == NULL) {
		return NULL;
	}

	while(!peek_token_is(parser, SEMICOLON) && precedence < peek_precedence(parser)){
		infix_parser infix_fn = parser_infix_fns(parser->peek_token.type);
		if (infix_fn == NULL){
			return left;
		}
		parser_next_token(parser);

		left = infix_fn(parser, left);
	}

	return left;
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
	printf("Whoops, we ran into some monkey business here!\n");
	printf("Parser Errors:\n");
	for (int i = 0; i < errors->count; i++){
		printf("%s\n", (char *)errors->data[i]);
	}
}

program_t *new_program(){
	program_t *program = malloc(sizeof(program_t));
	program->statements = create_vector();
	program->node_type = NODE_PROGRAM;
	return program;
}

program_t *parse_program(parser_t *parser){
	static bool globals_initialized = false;
	if (!globals_initialized) {
		init_globals();
		globals_initialized = true;
	}
	program_t *program = new_program();

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
	append_vector(program->statements, statement);
	return program;
}

prefix_parser parse_prefix_fns(TokenType token_type){
	switch(token_type){
		case IDENT:
			return &parse_identifier;
		case INT:
			return &parse_integer_literal;
		case BANG:
			return &parse_prefix_expression;
		case MINUS:
			return &parse_prefix_expression;
		case TRUE:
			return &parse_boolean;
		case FALSE:
			return &parse_boolean;
		case LPAREN:
			return &parse_group_expression;
		case IF:
			return &parse_if_expression;
		case FUNCTION:
			return &parse_function_literal;
		case STRING:
			return &parse_string_literal;
		case LBRACKET:
		  return &parse_array_literal;
		default:
			return NULL;
	}
}

infix_parser parser_infix_fns(TokenType token_type){
	switch (token_type) {
		case LPAREN:
			return &parse_call_expression;
		case LBRACKET:
			return &parse_index_expression;
		default:
			return &parse_infix_expression;
	}
}

expression_t *parse_index_expression(parser_t *parser, expression_t *left){
	token_t token = {
			.type = parser->curr_token.type,
			.literal = strdup(parser->curr_token.literal)
		};

	parser_next_token(parser);
	expression_t *expression = new_expression(INDEX_EXPR, token);
	expression->index_expression.left = left;
	expression->index_expression.index = parse_expression(parser, PRECEDENCE_LOWEST);

	if (!expect_peek(parser, RBRACKET)){
		return NULL;
	}
	return expression;
}

expression_t *parse_array_literal(parser_t *parser){
	token_t token = {
		.type = LBRACKET,
		.literal = strdup("[")
	};

	expression_t *array = new_expression(ARRAY_LITERAL, token);
	if (peek_token_is(parser, RBRACKET)) {
		parser_next_token(parser);
		array->array_literal.elements = create_vector();
		return array;
	}
	vector_t *list = parse_expression_list(parser, RBRACKET);
	array->array_literal.elements = list;

	return array;
}

expression_t *parse_identifier(parser_t *parser){
	token_t token = {
			.type = IDENT,
			.literal = parser->curr_token.literal
		};
	expression_t *expression = new_expression(IDENT_EXPR, token);
	expression->ident.token = token;
	expression->ident.value = token.literal;
	return expression;
};

expression_t *parse_integer_literal(parser_t *parser){
	token_t token = {
			.type = INT,
			.literal = parser->curr_token.literal
		};
	expression_t *expression = new_expression(INTEGER_LITERAL, token);
	expression->integer = atoi(token.literal);
	return expression;
};

expression_t *parse_prefix_expression(parser_t *parser){
	token_t token = {
			.type = parser->curr_token.type,
			.literal = strdup(parser->curr_token.literal)
		};
	expression_t *expression = new_expression(PREFIX_EXPR, token);
	expression->prefix_expression.op = strdup(token.literal);
	parser_next_token(parser);

	expression->prefix_expression.right = parse_expression(parser, PRECEDENCE_PREFIX);
	return expression;

}

expression_t *parse_boolean(parser_t *parser){
	token_t token = {
			.type = parser->curr_token.type,
			.literal = strdup(parser->curr_token.literal)
	};

	expression_t *expression = new_expression(BOOLEAN_EXPR, token);
	if (strcmp(expression->token.literal, "true") == 0){
		expression->boolean = true;
	} else {
		expression->boolean = false;
	}
	return expression;

}

expression_t *parse_infix_expression(parser_t *parser, expression_t *left){
	token_t token = {
			.type = parser->curr_token.type,
			.literal = strdup(parser->curr_token.literal)
		};
	expression_t *expression = new_expression(INFIX_EXPR, token);
	expression->infix_expression.op = token.literal;
	expression->infix_expression.left = left;
	precedence_t precedence = curr_precedence(parser);
	parser_next_token(parser);
	expression->infix_expression.right = parse_expression(parser, precedence);
	return expression;
}

expression_t *parse_call_expression(parser_t *parser, expression_t *left){
	token_t token = {
			.type = parser->curr_token.type,
			.literal = strdup(parser->curr_token.literal)
		};
	expression_t *expression = new_expression(CALL_EXPRESSION, token);
	expression->call_expression.function = left;
	expression->call_expression.arguments = parse_expression_list(parser, RPAREN);
	return expression;
}

vector_t *parse_expression_list(parser_t *parser, TokenType end){
	vector_t *arguments = create_vector();

	if (peek_token_is(parser, RPAREN)){
		parser_next_token(parser);
		return arguments;
	}

	parser_next_token(parser);
	append_vector(arguments, parse_expression(parser, PRECEDENCE_LOWEST));

	while(peek_token_is(parser, COMMA)){
		parser_next_token(parser);
		parser_next_token(parser);
		append_vector(arguments, parse_expression(parser, PRECEDENCE_LOWEST));
	}

	if (!expect_peek(parser, end)){
		return NULL;
	}

	return arguments;
}

expression_t *parse_group_expression(parser_t *parser){
	parser_next_token(parser);
	expression_t *expression = parse_expression(parser, PRECEDENCE_LOWEST);
	if (!expect_peek(parser, RPAREN)){
		return NULL;
	}
	return expression;
}

expression_t *parse_if_expression(parser_t *parser){
	token_t token = {
			.type = parser->curr_token.type,
			.literal = strdup(parser->curr_token.literal)
		};
	expression_t *expression = new_expression(IF_EXPR, token);
	if (!expect_peek(parser, LPAREN)){
		return NULL;
	}
	parser_next_token(parser);
	expression->if_expression.condition = parse_expression(parser, PRECEDENCE_LOWEST);

	if (!expect_peek(parser, RPAREN)){
		return NULL;
	}
	if (!expect_peek(parser, LBRACE)){
		return NULL;
	}

	expression->if_expression.consequence = parse_block_statement(parser);

	if (peek_token_is(parser, ELSE)){
		parser_next_token(parser);

		if (!expect_peek(parser, LBRACE)){
			return NULL;
		}
		expression->if_expression.alternative = parse_block_statement(parser);
	}
	return expression;
}

expression_t *parse_function_literal(parser_t *parser){
	token_t token = {
			.type = parser->curr_token.type,
			.literal = strdup(parser->curr_token.literal)
		};
	expression_t *expression = new_expression(FUNCTION_LITERAL, token);
	if (!expect_peek(parser, LPAREN)){
		return NULL;
	}

	expression->function_literal.parameters = parse_function_parameters(parser);
	if (!expect_peek(parser, LBRACE)){
		return NULL;
	}

	expression->function_literal.body = parse_block_statement(parser);
	return expression;
}

expression_t *parse_string_literal(parser_t*parser){
	expression_t *expression = new_expression(STRING_LITERAL, parser->curr_token);
	expression->string_literal = string_from(parser->curr_token.literal);
	return expression;
}

vector_t *parse_function_parameters(parser_t *parser) {
	vector_t *parameters = create_vector();  // Initialize vector if not already done

	if (peek_token_is(parser, RPAREN)) {
		parser_next_token(parser);
		return parameters;
	}

	parser_next_token(parser);

	// Allocate identifier and add to vector
	identifier_t *identifier = malloc(sizeof(identifier_t));
	identifier->value = strdup(parser->curr_token.literal);
	identifier->token.type = parser->curr_token.type;
	identifier->token.literal = strdup(parser->curr_token.literal);
	append_vector(parameters, identifier);

	// Parse any additional parameters
	while (peek_token_is(parser, COMMA)) {
		parser_next_token(parser);  // consume comma
		parser_next_token(parser);  // consume comma

		identifier_t *identifier = malloc(sizeof(identifier_t));
		identifier->value = strdup(parser->curr_token.literal);
		identifier->token.type = parser->curr_token.type;
		identifier->token.literal = strdup(parser->curr_token.literal);
		append_vector(parameters, identifier);
	}

	if (!expect_peek(parser, RPAREN)) {
		return parameters;
	}
	return parameters;
}

block_statement_t *parse_block_statement(parser_t *parser){
	block_statement_t *block_statement = new_block_statement();
	token_t token = {
			.type = parser->curr_token.type,
			.literal = strdup(parser->curr_token.literal)
		};
	block_statement->token = token;

	parser_next_token(parser);

	while(!curr_token_is(parser, RBRACE) && !curr_token_is(parser, EOF_TOKEN)){
		statement_t *statement = parse_statement(parser);
		if (statement != NULL){
			append_vector(block_statement->statements, statement);
		}
	parser_next_token(parser);
	}
	return block_statement;
}

precedence_t peek_precedence(parser_t *parser){
	return get_precedence(parser->peek_token.type);
}

precedence_t curr_precedence(parser_t *parser){
	return get_precedence(parser->curr_token.type);
}

precedence_t get_precedence(TokenType token_type) {
    switch (token_type) {
        case EQ:
            return PRECEDENCE_EQUALS;
        case NOT_EQ:
            return PRECEDENCE_NOT_EQUALS;
        case LT:
        case GT:
            return PRECEDENCE_LESSGREATER;
        case PLUS:
        case MINUS:
            return PRECEDENCE_SUM;
        case ASTERISK:
        case SLASH:
            return PRECEDENCE_PRODUCT;
        case LPAREN:
            return PRECEDENCE_CALL;
		case LBRACKET:
			return PRECEDENCE_INDEX;
        default:
            return PRECEDENCE_LOWEST;
    }
}
