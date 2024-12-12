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

#endif
