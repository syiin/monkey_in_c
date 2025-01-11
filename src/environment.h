#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "evaluator.h"
#include "hashmap.h"

typedef struct Environment environment_t;
typedef struct Environment {
	hash_map_t *table;
	environment_t *outer;
} environment_t;

environment_t *new_environment();
void free_object(void *object);

#endif
