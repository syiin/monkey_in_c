#include "environment.h"
#include "evaluator.h"
#include "hashmap.h"
#include "object.h"

environment_t *new_environment(){
    environment_t *environment = malloc(sizeof(environment_t));
    environment->table = new_hash_table(free_object);
    environment->outer = NULL;
    return environment;
}

bool env_set(environment_t *env, char *key, object_t *object){
    object_t *copy = object_heap_copy(object);
    return hash_set(env->table, key, copy);
}

object_t *env_get(environment_t *env, char *key){
    object_t *object = (object_t *)hash_get(env->table, key); 
    if (object == NULL && env->outer != NULL){
        object = env_get(env->outer, key);
    }
    return object;
}

void free_object(void *object){
    free((object_t *)(object));
}
