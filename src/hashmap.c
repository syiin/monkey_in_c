#include "hashmap.h"
#include <stdint.h>
#include <string.h>

hash_map_t *new_hash_table(free_value_t free_fn){
    hash_map_t *hash_map = malloc(sizeof(hash_map_t));
    hash_map->table = calloc(TABLE_SIZE, sizeof(hash_entry_t*));
    hash_map->free_value = free_fn;
    return hash_map;
}

void hash_set(hash_map_t *hash_map, char *key, void *value){
    uint64_t idx = fnv1a_hash(key);
    hash_entry_t *curr_entry = hash_map->table[idx];
    while(curr_entry != NULL){
        curr_entry = curr_entry->next;
        if(strcmp(curr_entry->key, key) == 0){
            curr_entry->value = value;
        }
    }

    hash_entry_t *entry = malloc(sizeof(hash_entry_t));
    entry->key = key;
    entry->value = value;
    entry->next = NULL;
    curr_entry->next = entry;
}

void *hash_get(hash_map_t *hash_map, char *key){
    uint64_t idx = fnv1a_hash(key);
    hash_entry_t *curr_entry = hash_map->table[idx];
    while(curr_entry != NULL){
        if(strcmp(curr_entry->key, key) == 0){
            return curr_entry->value;
        }
    }
    return NULL;
}

uint64_t fnv1a_hash(const char* str) {
    uint64_t hash = 0xcbf29ce484222325ULL; // FNV-1a offset basis

    while (*str) {
        hash ^= (uint64_t)*str++;
        hash *= 0x100000001B3ULL; // FNV-1a prime
    }

    return hash;
}

void free_hash(hash_map_t *hash_map){
    for(int i = 0; i < TABLE_SIZE; i++){
        hash_entry_t *curr_entry = hash_map->table[i];
        while(curr_entry != NULL){
            hash_entry_t *next = curr_entry->next;
            free(curr_entry->key);
            if (hash_map->free_value != NULL){
                hash_map->free_value(curr_entry->value);
            }
            free(curr_entry);
            curr_entry = next;
        }
    }
 
}
