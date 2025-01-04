#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <stdbool.h>
#include "ast.h"

typedef enum ObjectType {
	OBJECT_INTEGER,
	OBJECT_BOOLEAN,
	OBJECT_NULL,
	OBJECT_RETURN,
	OBJECT_ERROR,
} object_type_t;

typedef struct Object object_t;
typedef struct Object {
	object_type_t type;
	union {
		int integer;
		bool boolean;
		void *null;
		char *error_message;
	};
} object_t;

static const object_t global_true = { .type = OBJECT_BOOLEAN, .boolean = true };
static const object_t global_false = { .type = OBJECT_BOOLEAN, .boolean = false };
static const object_t global_null = { .type = OBJECT_NULL, .null = NULL};

char *object_type_to_string(object_type_t object_type);
object_t eval(void *node, node_type_t node_type);
object_t eval_program(program_t *program);
object_t eval_expression_node(expression_t *expression);
object_t eval_infix_expression(char *op, object_t left, object_t right);
object_t eval_integer_infix_expression(char *op, object_t left, object_t right);
object_t native_bool_to_boolean(bool input);
object_t eval_prefix_expression(char *op, object_t right);
object_t eval_bang_operator(object_t right);
void inspect_object(object_t object, char *buff_out);
object_t eval_minus_operator(object_t right);
bool is_truthy(object_t object);
object_t eval_statement(statement_t *statement);
object_t eval_block_statement(block_statement_t *statement);
#endif
