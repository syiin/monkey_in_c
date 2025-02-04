#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include "ast.h"
#include "custom_string.h"
#include "hashmap.h"
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
	OBJECT_ARRAY,
	OBJECT_HASH,
} object_type_t;

typedef struct Array{
	vector_t *elements;
} array_object_t;

typedef struct Function{
	environment_t *env;
	vector_t *parameters;
	block_statement_t *body;
} function_object_t;

typedef struct Hash{
	hash_map_t *pairs;
} hash_object_t;

typedef object_t *(*builtin_function_t)(vector_t *args);
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
		array_object_t array;
		hash_object_t hash;
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

char *object_to_key(object_t *object);
char *object_to_formattable_key(object_t *object);
const char *strip_object_prefix(const char *key);

#endif
