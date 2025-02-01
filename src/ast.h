#ifndef AST_H
#define AST_H

#include "hashmap.h"
#include "token.h"
#include "custom_string.h"
#include "vector.h"

typedef enum {
	NODE_EXPRESSION,
	NODE_STATEMENT,
	NODE_BLOCK_STATEMENT,
	NODE_PROGRAM,
} node_type_t;

typedef enum {
	LET_STATEMENT,
	RETURN_STATEMENT,
	EXPRESSION_STATEMENT,
} statement_type_t;

typedef enum {
	IDENT_EXPR,
	INTEGER_LITERAL,
	PREFIX_EXPR,
	INFIX_EXPR,
	BOOLEAN_EXPR,
	IF_EXPR,
	FUNCTION_LITERAL,
	CALL_EXPRESSION,
	STRING_LITERAL,
	ARRAY_LITERAL,
	HASH_LITERAL,
	INDEX_EXPR,
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

typedef struct ArrayLiteral {
	vector_t *elements;
} array_literal_t;

typedef struct HashLiteral {
	hash_map_t *pairs;
} hash_literal_t;

typedef struct CallExpression {
	expression_t *function;
	vector_t *arguments;
} call_expression_t;

typedef struct IndexExpression {
	token_t token;
	expression_t *left;
	expression_t *index;
} index_expression_t;

typedef struct Expression {
	node_type_t node_type;
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
		string_t *string_literal;
		array_literal_t array_literal;
		index_expression_t index_expression;
		hash_literal_t hash_literal;
	};
} expression_t;

typedef struct Statement {
	node_type_t node_type;
	statement_type_t type;
	token_t token;
	identifier_t name;
	expression_t *value;
} statement_t;

typedef struct BlockStatement {
	node_type_t node_type;
	token_t token;
	vector_t *statements;
} block_statement_t;

typedef struct Program{
	node_type_t node_type;
	vector_t *statements;
} program_t;

void token_literal(program_t *program);
void format_statement(string_t *str_buffer, statement_t *statement);
void format_expression_statement(string_t *str, expression_t *expression);
void format_block_statement(string_t *str, block_statement_t *block);
void ast_string(string_t *format_buffer, program_t *program);
char *program_to_string(program_t *program);
expression_t *new_expression(expression_type_t type, token_t token);
statement_t *new_statement(statement_type_t type);
block_statement_t *new_block_statement();
#endif
