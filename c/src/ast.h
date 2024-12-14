#ifndef AST_H
#define AST_H

#include "token.h"

typedef enum {
	LET_STATEMENT,
	RETURN_STATEMENT,
	EXPRESSION_STATEMENT,
} StatementType;

typedef enum {
	IDENT_EXPR,
	INTEGER_LITERAL,
} ExpressionType;

typedef struct Identifier {
	token_t token;
	char *value;
} identifier_t;

typedef struct Expression {
	ExpressionType type;
	token_t token;
	union {
		int integer;
		identifier_t ident;
	};
} expression_t;

typedef struct Statement {
	StatementType type;
	token_t token;
	identifier_t name;
	expression_t *value;
} statement_t;

typedef struct Program{
	int statement_cap;
	int count;
	statement_t *statements[];
} program_t;

void token_literal(program_t *program);
void print_ast(program_t *program);
void format_statement(char *str_buffer, int buffer_size, statement_t *statement);
void ast_string(char *str_buffer, program_t *program);
expression_t *new_expression(ExpressionType type, token_t token);

#endif
