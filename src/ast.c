#include "token.h"
#include "custom_string.h"
#include "ast.h"
#include "hashmap.h"
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
	for (int i = 0; i < program->statements->count; i++){
		print_token(&((statement_t *)program->statements->data[i])->token);
	};
}

void ast_string(string_t *format_buffer, program_t *program){
	for (int i = 0; i < program->statements->count; i++){
		statement_t *statement = program->statements->data[i];
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
            format_expression_statement(str_buffer, statement->value);
            break;
        }
        case RETURN_STATEMENT: {
            string_append(str_buffer, statement->token.literal);
            string_append(str_buffer, " ");
            if (statement->value != NULL){
                format_expression_statement(str_buffer, statement->value);
            }
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
        case BOOLEAN_EXPR:
            string_append(str, expression->token.literal);
            break;
        case IF_EXPR:
            string_append(str, "if");
            format_expression_statement(str, expression->if_expression.condition);
            string_append(str, " ");
            format_block_statement(str, expression->if_expression.consequence);
            if (expression->if_expression.alternative != NULL) {
                string_append(str, "else ");
                format_block_statement(str, expression->if_expression.alternative);
            }
            break;
        case FUNCTION_LITERAL:
            string_append(str, expression->token.literal);
            string_append(str, "(");
            for (size_t i = 0; i < expression->function_literal.parameters->count; i++) {
                identifier_t *ident = (identifier_t *)expression->function_literal.parameters->data[i];
                string_append(str, ident->token.literal);
                if (i < expression->function_literal.parameters->count - 1) {
                    string_append(str, ", ");
                }
            }
            string_append(str, ") ");
            format_block_statement(str, expression->function_literal.body);
            break;
        case CALL_EXPRESSION:
            format_expression_statement(str, expression->call_expression.function);
            string_append(str, "(");
            for (size_t i = 0; i < expression->call_expression.arguments->count; i++){
                expression_t *expr = (expression_t *)expression->call_expression.arguments->data[i];
                if (i > 0) {
                    string_append(str, ", ");
                }
                format_expression_statement(str, expr);
            }
            string_append(str, ")");
            break;
        case STRING_LITERAL:
            string_append(str, "\"");
            string_append(str, expression->token.literal);
            string_append(str, "\"");
            break;
        case ARRAY_LITERAL:
            string_append(str, "[");
            for (size_t i = 0; i < expression->array_literal.elements->count; i++){
                if (i > 0) {
                    string_append(str, ", ");
                }
                format_expression_statement(str, expression->array_literal.elements->data[i]);
            }
            string_append(str, "]");
            break;
        case HASH_LITERAL:
            string_append(str, "{");
            bool first = true;
            parser_hash_literal_t hash_literal = expression->hash_literal;

            // Iterate through all buckets
            for (int i = 0; i < hash_literal.pairs_len; i++) {
                parser_hash_pair_t *entry = hash_literal.pairs[i];
                while (entry != NULL) {
                    if (!first) {
                        string_append(str, ", ");
                    }
                    first = false;

                    // Format key (assuming it's a string)
                    string_append(str, "\"");
                    format_expression_statement(str, (expression_t *)entry->key);
                    string_append(str, "\": ");

                    // Format value (assuming it's an expression)
                    format_expression_statement(str, (expression_t *)entry->value);
                }
            }
            string_append(str, "}");
            break;
        case INDEX_EXPR: {
            string_append(str, "(");
            format_expression_statement(str, expression->index_expression.left);
            string_append(str, "[");
            format_expression_statement(str, expression->index_expression.index);
            string_append(str, "])");
            break;
        }
    }
}

void format_block_statement(string_t *str, block_statement_t *block) {
    for (int i = 0; i < block->statements->count; i++) {
        format_statement(str, block->statements->data[i]);
    }
}

expression_t *new_expression(expression_type_t type, token_t token){
    expression_t *expression = malloc(sizeof(expression_t));
    expression->token = token;
    expression->type = type;
    expression->node_type = NODE_EXPRESSION;
    return expression;
}

statement_t *new_statement(statement_type_t type){
    statement_t *statement = malloc(sizeof(statement_t));
    if (statement == NULL){
        return NULL;
    }
    statement->node_type = NODE_STATEMENT;
    statement->type = type;
    return statement;
}

block_statement_t *new_block_statement(){
    block_statement_t *block_statement = malloc(sizeof(block_statement_t));
    block_statement->statements = create_vector();
    block_statement->node_type = NODE_BLOCK_STATEMENT;
    return block_statement;
}
