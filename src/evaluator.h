#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <stdbool.h>
#include "ast.h"

typedef enum ObjectType {
	OBJECT_INTEGER,
	OBJECT_BOOLEAN,
	OBJECT_NULL,
} object_type_t;

typedef struct Object {
	object_type_t type;
	union {
		int integer;
		bool boolean;
		void *null;
	};
} object_t;

char *object_type_to_string(object_type_t object_type);
object_t eval(void *node, node_type_t node_type);
object_t eval_expression_node(expression_t *expression);
object_t eval_program_node(program_t *program);
object_t eval_prefix_expression(char *op, object_t right);
object_t eval_bang_operator(object_t right);
void inspect_object(object_t object, char *buff_out);
object_t eval_minus_operator(object_t right);
#endif
