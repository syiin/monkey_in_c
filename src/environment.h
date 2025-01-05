#include "evaluator.h"
#include "hashmap.h"

typedef struct Environment environment_t;
typedef struct Environment {
	hash_map_t *table;
	environment_t *outer;
} environment_t;
