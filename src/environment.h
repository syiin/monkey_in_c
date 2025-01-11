#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "hashmap.h"
#include "object.h"

typedef struct Environment environment_t;
typedef struct Environment {
	hash_map_t *table;
	environment_t *outer;
} environment_t;

environment_t *new_environment();
bool env_set(environment_t *env, char *key, object_t *object);
object_t *env_get(environment_t *env, char *key);
void free_object(void *object);

#endif
