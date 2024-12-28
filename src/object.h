#ifndef OBJECT_H
#define OBJECT_H

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
object_t eval(expression_t *expression);

#endif
