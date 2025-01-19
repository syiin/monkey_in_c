#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <stdbool.h>
#include "ast.h"
#include "object.h"

typedef struct Environment environment_t;

char *object_type_to_string(object_type_t object_type);
object_t* eval(void *node, node_type_t node_type, environment_t *env);
object_t* eval_program(program_t *program, environment_t *env);
object_t* eval_expression_node(expression_t *expression, environment_t *env);
object_t* eval_infix_expression(char *op, object_t *left, object_t *right);
object_t* eval_integer_infix_expression(char *op, object_t *left, object_t *right); 
object_t *eval_string_infix_expression(char *op, object_t *left, object_t *right);
object_t* native_bool_to_boolean(bool input);
object_t* eval_prefix_expression(char *op, object_t *right);
object_t* eval_bang_operator(object_t *right);
object_t* eval_minus_operator(object_t *right);
bool is_truthy(object_t *object);
object_t* eval_statement(statement_t *statement, environment_t *env);
object_t* eval_block_statement(block_statement_t *statement, environment_t *env);
object_t* new_error(char *format);
vector_t *eval_call_expressions(vector_t *input_args, environment_t *env);
object_t *apply_function(object_t *fn, vector_t *args);
#endif
