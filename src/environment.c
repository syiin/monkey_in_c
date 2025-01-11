#include "environment.h"
#include "evaluator.h"
#include "hashmap.h"

environment_t *new_environment(){
    environment_t *environment = malloc(sizeof(environment_t));
    environment->table = new_hash_table(free_object);
    return environment;
}

bool env_set(environment_t *env, char *key, object_t *object){
    return hash_set(env->table, key, object);
}

object_t *env_get(environment_t *env, char *key){
    return (object_t *)hash_get(env->table, key);
}

void free_object(void *object){
    free((object_t *)(object));
}
