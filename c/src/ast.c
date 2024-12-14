#include "token.h"
#include "ast.h"
#include <err.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void token_literal(program_t *program){
	for (int i = 0; i < program->count; i++){
		print_token(&program->statements[i]->token);
	};
}

void ast_string(char *str_buffer, program_t *program){
	for (int i = 0; i < program->count; i++){
		statement_t *statement = program->statements[i];
		char statement_str[256];
		format_statement(statement_str, 256, statement);
		strcat(str_buffer, statement_str);
	};
}

void format_statement(char *str_buffer, int buffer_size, statement_t *statement){
	if (str_buffer == NULL){
		err(EXIT_FAILURE, "OUT OF MEMORY");
	}
	snprintf(str_buffer, buffer_size, "");
	switch(statement->type){
		case LET_STATEMENT:
		    snprintf(str_buffer + strlen(str_buffer), buffer_size - strlen(str_buffer),
			     "%s %s = %s;",
			     statement->token.literal,
			     statement->name.value,
			     statement->value->ident.value);
			break;
		case RETURN_STATEMENT:
		    snprintf(str_buffer + strlen(str_buffer), buffer_size - strlen(str_buffer),
			     "%s %s;",
			     statement->token.literal,
			     statement->value->ident.value);
			break;
		default:
			return;
	}
}

expression_t *new_expression(ExpressionType type, token_t token){
	expression_t *expression = malloc(sizeof(expression_t));
	expression->token = token;
	expression->type = type;
	return expression;
}
