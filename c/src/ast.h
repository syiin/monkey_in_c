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
	PREFIX_EXPR,
	INFIX_EXPR,
} ExpressionType;

typedef struct Identifier {
	token_t token;
	char *value;
} identifier_t;

typedef struct Expression expression_t;

typedef struct PrefixExpression {
	char *op;
	expression_t *right;
} prefix_expression_t;

typedef struct InfixExpression {
	char *op;
	expression_t *right;
	expression_t *left;
} infix_expression_t;

typedef struct Expression {
	ExpressionType type;
	token_t token;
	union {
		int integer;
		identifier_t ident;
		prefix_expression_t prefix_expression;
		infix_expression_t infix_expression;
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
void format_statement(char *str_buffer, statement_t *statement);
void format_expression_statement(char *str_buffer, expression_t *expression);
void ast_string(char *str_buffer, program_t *program);
expression_t *new_expression(ExpressionType type, token_t token);

#endif
