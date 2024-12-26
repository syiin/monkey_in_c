#ifndef AST_H
#define AST_H

#include "token.h"
#include "custom_string.h"
#include "vector.h"

typedef enum {
	LET_STATEMENT,
	RETURN_STATEMENT,
	EXPRESSION_STATEMENT,
	BLOCK_STATEMENT,
} statement_type_t;

typedef enum {
	IDENT_EXPR,
	INTEGER_LITERAL,
	PREFIX_EXPR,
	INFIX_EXPR,
	BOOLEAN_EXPR,
	IF_EXPR,
	FUNCTION_LITERAL,
	CALL_EXPRESSION
} expression_type_t;

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

typedef struct BlockStatement block_statement_t;
typedef struct Expression expression_t;

typedef struct IfExpression {
	expression_t *condition;
	block_statement_t *consequence;
	block_statement_t *alternative;
} if_expression_t;

typedef struct FunctionLiteral {
	block_statement_t *body;
	vector_t *parameters;
} function_literal_t;

typedef struct CallExpression {
	expression_t *function;
	vector_t *arguments;
} call_expression_t;

typedef struct Expression {
	expression_type_t type;
	token_t token;
	union {
		int integer;
		bool boolean;
		identifier_t ident;
		prefix_expression_t prefix_expression;
		infix_expression_t infix_expression;
		if_expression_t if_expression;
		function_literal_t function_literal;
		call_expression_t call_expression;
	};
} expression_t;

typedef struct Statement {
	statement_type_t type;
	token_t token;
	identifier_t name;
	expression_t *value;
} statement_t;

typedef struct BlockStatement {
	token_t token;
	vector_t *statements;
} block_statement_t;

typedef struct Program{
	vector_t *statements;
} program_t;

void token_literal(program_t *program);
void print_ast(program_t *program);
void format_statement(string_t *str_buffer, statement_t *statement);
void format_expression_statement(string_t *str, expression_t *expression);
void format_block_statement(string_t *str, block_statement_t *block);
void ast_string(string_t *format_buffer, program_t *program);
char *program_to_string(program_t *program);
expression_t *new_expression(expression_type_t type, token_t token);

#endif
