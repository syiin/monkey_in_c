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
	OBJECT_STRING,
	OBJECT_BUILTIN,
} object_type_t;

typedef struct Function{
	environment_t *env;
	vector_t *parameters;
	block_statement_t *body;
} function_object_t;

typedef object_t *(*builtin_function_t)(int count, ...);
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
		string_t *string_literal;
		builtin_function_t builtin;
	};
} object_t;

extern object_t *global_true;
extern object_t *global_false;
extern object_t *global_null;

void init_globals();
object_t *new_object(object_type_t obj_type);
void inspect_object(object_t object, char *buff_out);
object_t *object_heap_copy(const object_t *source);
object_t *get_builtin_by_name(const char *name);

#endif
