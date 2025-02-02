#include "vector.h"
#include <stdlib.h>
#include <stdint.h>

vector_t *create_vector(){
	int init_cap = 2;
	vector_t *vector = malloc(sizeof(vector_t));
	if (vector == NULL){
		return NULL;
	}

	vector->data =  malloc(sizeof(void *) * init_cap);
	if (vector->data == NULL){
		free(vector);
		return NULL;
	}

	vector->capacity = init_cap;
	vector->count = 0;
	return vector;
}

void append_vector(vector_t *vector, void *element){
	if (vector->count >= vector->capacity){
		vector->capacity = vector->capacity * 2;
		void **tmp = realloc(vector->data, sizeof(void *) * vector->capacity);
		vector->data = tmp;
	}
	vector->data[vector->count] = element;
	vector->count++;
}

vector_t *rest_vector(const vector_t *original, copy_fn_t copy){
	vector_t *copied = create_vector();
	if (copied == NULL){
		return NULL;
	}

	if (original == NULL || original->count <= 1){
		return copied;
	}
	
	int new_count = original->count - 1;
	int new_capacity = new_count * 2;
	if (new_capacity > SIZE_MAX/sizeof(void *)){
		return NULL;
	}

	copied->data =  malloc(sizeof(void *) * copied->capacity);
	if (copied->data == NULL){
		free(copied);
		return NULL;
	}

	for (int i = 1; i < original->count; i++){
		copied->data[i-1] = copy( original->data[i] );
	}

	copied->capacity = new_capacity;
	copied->count = new_count;
	return copied;
}

void free_vector(vector_t *vector){
	for (int i = 0; i < vector->count; i++){
		free(vector->data[i]);
	}
	free(vector);
}
