#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include "custom_string.h"

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
		string_t *error_message;
	};
} object_t;

static const object_t global_true = { .type = OBJECT_BOOLEAN, .boolean = true };
static const object_t global_false = { .type = OBJECT_BOOLEAN, .boolean = false };
static const object_t global_null = { .type = OBJECT_NULL, .null = NULL};

#endif