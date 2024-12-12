#ifndef AST_H
#define AST_H

#include "token.h"

typedef enum {
	LET_STATEMENT,
} StatementType;

typedef struct Expression {
	token_t token; 
} expression_t;

typedef struct Identifier {
	token_t token; //IDENT
	char *value;
} identifier_t;

typedef struct Statement {
	StatementType type;
	token_t token; //LET
	identifier_t name;
	expression_t *value;
} statement_t;

typedef struct Program{
	/*statement_t **statements;*/
	int statement_cap;
	int count;
	statement_t *statements[];
} program_t;

void token_literal(program_t *program);

#endif
