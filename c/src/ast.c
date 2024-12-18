#include "token.h"
#include "custom_string.h"
#include "ast.h"
#include <err.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *program_to_string(program_t *program) {
    string_t *str_buffer = string_new();
    if (!str_buffer) {
        return NULL;
    }

    ast_string(str_buffer, program);
    char *result = string_get_data(str_buffer);
    
    return result;
}

void token_literal(program_t *program){
	for (int i = 0; i < program->count; i++){
		print_token(&program->statements[i]->token);
	};
}

void ast_string(string_t *format_buffer, program_t *program){
	for (int i = 0; i < program->count; i++){
		statement_t *statement = program->statements[i];
		format_statement(format_buffer, statement);
	};
}

void format_statement(string_t *str_buffer, statement_t *statement) {
    if (str_buffer == NULL) {
        err(EXIT_FAILURE, "OUT OF MEMORY");
    }

    if (statement->type != EXPRESSION_STATEMENT || str_buffer->len == 0) {
        string_clear(str_buffer);
    }

    switch(statement->type) {
        case LET_STATEMENT: {
            string_append(str_buffer, statement->token.literal);
            string_append(str_buffer, " ");
            string_append(str_buffer, statement->name.value);
            string_append(str_buffer, " = ");
            string_append(str_buffer, statement->value->ident.value);
            break;
        }
        case RETURN_STATEMENT: {
            string_append(str_buffer, statement->token.literal);
            string_append(str_buffer, " ");
            string_append(str_buffer, statement->value->ident.value);
            break;
        }
        case EXPRESSION_STATEMENT: {
            format_expression_statement(str_buffer, statement->value);
            break;
        }
       default:
            return;
    }
}

void format_expression_statement(string_t *str, expression_t *expression) {
    switch(expression->type) {
        case IDENT_EXPR:
            string_append(str, expression->token.literal);
            break;
        case INTEGER_LITERAL:
            string_append(str, expression->token.literal);
            break;
        case PREFIX_EXPR:
            string_append(str, "(");
            string_append(str, expression->prefix_expression.op);
            format_expression_statement(str, expression->prefix_expression.right);
            string_append(str, ")");
            break;
        case INFIX_EXPR:
            string_append(str, "(");
            format_expression_statement(str, expression->infix_expression.left);
            string_append(str, " ");
            string_append(str, expression->infix_expression.op);
            string_append(str, " ");
            format_expression_statement(str, expression->infix_expression.right);
            string_append(str, ")");
            break;
    }
}

expression_t *new_expression(ExpressionType type, token_t token){
	expression_t *expression = malloc(sizeof(expression_t));
	expression->token = token;
	expression->type = type;
	return expression;
}
