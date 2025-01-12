#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include "ast.h"
#include "custom_string.h"
#include "vector.h"
#include "environment.h"

typedef enum ObjectType {
	OBJECT_INTEGER,
	OBJECT_BOOLEAN,
	OBJECT_NULL,
	OBJECT_RETURN,
	OBJECT_ERROR,
	OBJECT_FUNCTION,
} object_type_t;

typedef struct Function{
	environment_t *env;
	vector_t *parameters;
	block_statement_t *body;
} function_object_t;

typedef struct Object object_t;
typedef struct Object {
	object_type_t type;
	union {
		int integer;
		bool boolean;
		void *null;
		string_t *error_message;
		function_object_t function;	
		object_t *return_obj;
	};
} object_t;

static const object_t global_true = { .type = OBJECT_BOOLEAN, .boolean = true };
static const object_t global_false = { .type = OBJECT_BOOLEAN, .boolean = false };
static const object_t global_null = { .type = OBJECT_NULL, .null = NULL};
void inspect_object(object_t object, char *buff_out);

object_t *object_heap_copy(const object_t *source);
#endif
