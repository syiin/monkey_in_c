#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct Vector {
	void **data;
	size_t capacity;
	size_t count;
} vector_t;

vector_t *create_vector();
void append_vector(vector_t *vector, void *element);

typedef void* (*copy_fn_t)(const void*);
vector_t *rest_vector(const vector_t *vector, copy_fn_t copy);

#endif
