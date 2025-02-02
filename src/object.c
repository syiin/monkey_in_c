#include "object.h"
#include "ast.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "custom_string.h"
#include "evaluator.h"
#include "vector.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

object_t *global_true;
object_t *global_false;
object_t *global_null;

void init_globals(void) {
    global_true = new_object(OBJECT_BOOLEAN);
    global_true->boolean = true;

    global_false = new_object(OBJECT_BOOLEAN);
    global_false->boolean = false;

    global_null = new_object(OBJECT_NULL);
}

object_t *new_object(object_type_t obj_type){
    object_t *obj = calloc(1, sizeof(object_t));
    if (obj == NULL){
        return NULL;
    }
    obj->type = obj_type;
    return obj;
}

void inspect_object(object_t object, char *buff_out){
    switch(object.type){
        case OBJECT_INTEGER:
            snprintf(buff_out, BUFSIZ, "%d", object.integer);
            break;
        case OBJECT_BOOLEAN:
            snprintf(buff_out, BUFSIZ, "%s", object.boolean ? "true" : "false");
            break;
        case OBJECT_NULL:
            snprintf(buff_out, BUFSIZ, "NULL");
            break;
        case OBJECT_FUNCTION:{
           string_t *params = string_new();
           for (int i = 0; i < object.function.parameters->count; i++) {
               identifier_t *param = object.function.parameters->data[i];
               string_append(params, param->value);
               if (i < object.function.parameters->count - 1) {
                   string_append(params, ", ");
               }
           }

           string_t *body_str = string_new();
           format_block_statement(body_str, object.function.body);

           snprintf(buff_out, BUFSIZ, "fn(%s) %s", params->data, body_str->data);

           string_free(params);
           string_free(body_str);
           break;
        }
        case OBJECT_STRING:{
            snprintf(buff_out, BUFSIZ, "%s", object.string_literal->data);
            break;
        }
        case OBJECT_ERROR:{
            snprintf(buff_out, BUFSIZ, "%s", object.error_message->data);
            break;
        }
        case OBJECT_ARRAY:{
            string_t *temp = string_new();
            string_append(temp, "[");
            for (int i = 0; i < object.array.elements->count; i++){
                object_t *element = object.array.elements->data[i];
                char element_str[BUFSIZ];
                inspect_object(*element, element_str);
                string_append(temp, element_str);
                if (i < object.array.elements->count - 1){
                    string_append(temp, ", ");
                }
            }
            string_append(temp, "]");
            snprintf(buff_out, BUFSIZ, "%s", temp->data);
            string_free(temp);
            break;
        }
        case OBJECT_HASH: {
            string_t *temp = string_new();
            string_append(temp, "{");
            bool first = true;

            // Iterate through all buckets in the hash table
            for (int i = 0; i < TABLE_SIZE; i++) {
                hash_entry_t *entry = object.hash.pairs->table[i];
                // Iterate through all entries in this bucket
                while (entry != NULL) {
                    if (!first) {
                        string_append(temp, ", ");
                    }
                    first = false;

                    // Add the key
                    string_append(temp, entry->key);
                    string_append(temp, ": ");

                    // Add the value
                    char value_str[BUFSIZ];
                    inspect_object(*(object_t*)entry->value, value_str);
                    string_append(temp, value_str);

                    entry = entry->next;
                }
            }
            string_append(temp, "}");
            snprintf(buff_out, BUFSIZ, "%s", temp->data);
            string_free(temp);
            break;
        }
        case OBJECT_BUILTIN: {
            snprintf(buff_out, BUFSIZ, "builtin function");
            break;
        }
       break;
            default:
                snprintf(buff_out, BUFSIZ, "Unknown object type");
                break;
        }
}

object_t *object_heap_copy(const object_t *source) {
    object_t *new_obj = malloc(sizeof(object_t));
    if (!new_obj) return NULL;

    memcpy(new_obj, source, sizeof(object_t));

    if (source->type == OBJECT_ERROR && source->error_message) {
        new_obj->error_message = string_clone(source->error_message);
        if (!new_obj->error_message) {
            free(new_obj);
            return NULL;
        }
    }

    return new_obj;
}

object_t *get_builtin_function(builtin_function_t builtin_function){
    object_t *builtin = new_object(OBJECT_BUILTIN);
    builtin->builtin = builtin_function;
    return builtin;
}

object_t *len_builtin(vector_t *args){
    if (args->count != 1){
        return new_error("wrong number of arguments");
    }
    object_t *arg = args->data[0];
    switch(arg->type){
        case OBJECT_STRING:{
            object_t *obj = new_object(OBJECT_INTEGER);
            obj->integer = arg->string_literal->len;
            return obj;
        }
        case OBJECT_ARRAY:{
            object_t *obj = new_object(OBJECT_INTEGER);
            obj->integer = arg->array.elements->count;
            return obj;
        }
        default:{
            char error_msg[BUFSIZ];
            snprintf(error_msg, BUFSIZ, "argument to `len` not supported, got %s", object_type_to_string(arg->type));
            return new_error(error_msg);
        }
    }
}


object_t *first_builtin(vector_t *args){
    if (args->count != 1){
        return new_error("wrong number of arguments");
    }
    object_t *arg = args->data[0];
    if (arg->type != OBJECT_ARRAY){
        char error_msg[BUFSIZ];
        snprintf(error_msg, BUFSIZ, "argument to `first` must be an array, got %s", object_type_to_string(arg->type));
        return new_error(error_msg);
    }

    if (arg->array.elements->count > 0){
        return arg->array.elements->data[0];
    }

    return global_null;
}

object_t *last_builtin(vector_t *args){
    if (args->count != 1){
        return new_error("wrong number of arguments");
    }
    object_t *arg = args->data[0];
    if (arg->type != OBJECT_ARRAY){
        char error_msg[BUFSIZ];
        snprintf(error_msg, BUFSIZ, "argument to `last` must be an array, got %s", object_type_to_string(arg->type));
        return new_error(error_msg);
    }

    int count = arg->array.elements->count;
    if (count > 0){
        return arg->array.elements->data[count - 1];
    }

    return global_null;
}

object_t *rest_builtin(vector_t *args){
    if (args->count != 1){
        return new_error("wrong number of arguments");
    }
    object_t *arg = args->data[0];
    if (arg->type != OBJECT_ARRAY){
        char error_msg[BUFSIZ];
        snprintf(error_msg, BUFSIZ, "argument to `rest` must be an array, got %s", object_type_to_string(arg->type));
        return new_error(error_msg);
    }

    object_t *new_array = object_heap_copy(arg);
    new_array->array.elements = create_vector();
    for(int i = 1; i < arg->array.elements->count; i++){
        append_vector(new_array->array.elements, object_heap_copy(arg->array.elements->data[i]));
    }

    return new_array;
}


object_t *push_builtin(vector_t *args){
    if (args->count != 2){
        return new_error("wrong number of arguments");
    }
    object_t *arg = args->data[0];
    if (arg->type != OBJECT_ARRAY){
        char error_msg[BUFSIZ];
        snprintf(error_msg, BUFSIZ, "argument to `rest` must be an array, got %s", object_type_to_string(arg->type));
        return new_error(error_msg);
    }

    object_t *new_array = object_heap_copy(arg);
    new_array->array.elements = create_vector();
    for(int i = 0; i < arg->array.elements->count; i++){
        append_vector(new_array->array.elements, object_heap_copy(arg->array.elements->data[i]));
    }

    append_vector(new_array->array.elements, object_heap_copy(args->data[1]));

    return new_array;
}


object_t *get_builtin_by_name(const char *name) {
    if (strcmp(name, "len") == 0) {
        object_t *built_in_obj = new_object(OBJECT_BUILTIN);
        built_in_obj->builtin = len_builtin;
        return built_in_obj;
    } else if (strcmp(name, "first") == 0) {
        object_t *built_in_obj = new_object(OBJECT_BUILTIN);
        built_in_obj->builtin = first_builtin;
        return built_in_obj;
    } else if(strcmp(name, "last") == 0){
        object_t *built_in_obj = new_object(OBJECT_BUILTIN);
        built_in_obj->builtin = last_builtin;
        return built_in_obj;
    } else if(strcmp(name, "rest") == 0){
        object_t *built_in_obj = new_object(OBJECT_BUILTIN);
        built_in_obj->builtin = rest_builtin;
        return built_in_obj;
    } else if(strcmp(name, "push") == 0){
        object_t *built_in_obj = new_object(OBJECT_BUILTIN);
        built_in_obj->builtin = push_builtin;
        return built_in_obj;
    }

    return NULL;
}

char *object_to_key(object_t *object){
    char buf[128];
    char *object_type = object_type_to_string(object->type);
    switch(object->type){
        case OBJECT_STRING:
            snprintf(buf, sizeof(buf), "%s_%s", object_type, object->string_literal->data);
            break;
        case OBJECT_INTEGER:
            snprintf(buf, sizeof(buf), "%s_%d", object_type, object->integer);
            break;
        case OBJECT_BOOLEAN:
            snprintf(buf, sizeof(buf), "%s_%s", object_type, object->boolean ? "true" : "false");
            break;
        default:
            printf("WARNING: INVALID OBJECT PASSED AS KEY\n");
    }
    return strdup(buf);
}

/*bool object_hash_set(object_hash_map_t *hash_map, object_t *key, object_t *value){*/
/*    uint64_t idx = 0;*/
/*    switch(key->type){*/
/*        case OBJECT_STRING: {*/
/*            uint64_t idx = fnv1a_hash(key->string_literal->data) % TABLE_SIZE;*/
/*            object_hash_entry_t *curr_entry = hash_map->table[idx];*/
/**/
/*            while(curr_entry != NULL){*/
/*                if(strcmp(curr_entry->key->string_literal->data, key->string_literal->data) == 0){*/
/*                    curr_entry->value = value;*/
/*                    return true;*/
/*                }*/
/*            }*/
/*            break;*/
/*        }*/
/*        case OBJECT_INTEGER: {*/
/*            uint64_t idx = key->integer;*/
/*            object_hash_entry_t *curr_entry = hash_map->table[idx];*/
/**/
/*            while(curr_entry != NULL){*/
/*                if(curr_entry->key->integer == key->integer){*/
/*                    curr_entry->value = value;*/
/*                    return true;*/
/*                }*/
/*            }*/
/*            break;*/
/*        }*/
/*        default:*/
/*            return false;*/
/*    }*/
/**/
/*    object_hash_entry_t *entry = malloc(sizeof(hash_entry_t));*/
/*    if (entry == NULL){*/
/*        return false;*/
/*    }*/
/*    entry->key = key;*/
/*    entry->value = value;*/
/*    entry->next = hash_map->table[idx];*/
/*    hash_map->table[idx] = entry;*/
/**/
/*    return true;*/
/*}*/
/**/
/*void *hash_get(hash_map_t *hash_map, object_t *key){*/
/*    uint64_t idx = fnv1a_hash(key) % TABLE_SIZE;*/
/*    hash_entry_t *curr_entry = hash_map->table[idx];*/
/*    while(curr_entry != NULL){*/
/*        if(strcmp(curr_entry->key, key) == 0){*/
/*            return curr_entry->value;*/
/*        }*/
/*        curr_entry = curr_entry->next;*/
/*    }*/
/*    return NULL;*/
/*}*/
/**/
/*uint64_t fnv1a_hash(const char* str) {*/
/*    uint64_t hash = 0xcbf29ce484222325ULL; // FNV-1a offset basis*/
/*    while (*str) {*/
/*        hash ^= (uint64_t)*str++;*/
/*        hash *= 0x100000001B3ULL; // FNV-1a prime*/
/*    }*/
/*    return hash;*/
/*}*/
