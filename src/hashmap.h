#include <stdlib.h>
#define TABLE_SIZE 128

typedef struct HashEntry hash_entry_t;
typedef struct HashEntry{
	char *key;
	void *value;
	hash_entry_t *next;
} hash_entry_t;

typedef void (*free_value_t)(void *);
typedef struct HashMap{
	hash_entry_t **table;
	free_value_t free_value;
} hash_map_t;

hash_map_t *new_hash_table(free_value_t free_fn);
void hash_set(hash_map_t *hash_map, char *key, void *value);
void *hash_get(hash_map_t *hash_map, char *key);
uint64_t fnv1a_hash(const char* str);
