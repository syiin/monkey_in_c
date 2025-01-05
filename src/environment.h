#include "evaluator.h"

typedef struct Entry entry_t;
typedef struct Entry {
	char *key;
	object_t *object;
	entry_t *next_entry;
} entry_t;

typedef struct Environment environment_t;
typedef struct Environment {
	entry_t **table;
	int size;
	environment_t *outer;
} environment_t;
