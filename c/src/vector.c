#include "vector.h"
#include <stdlib.h>

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
	vector->count++;
	vector->data[vector->count] = element;
}

void free_vector(vector_t *vector){
	free(vector->data);
	free(vector);
}
