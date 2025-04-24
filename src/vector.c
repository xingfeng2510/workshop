#include <stdlib.h>

#include "vector.h"
#include "mul.h"
Vector *vector_create()
{
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(NUM/MAXTHREADS*NUM * sizeof(int));
    vec->size = 0;
    vec->capacity = NUM/MAXTHREADS*NUM;
    return vec;
}

void vector_append(Vector *vec, int value)
{
    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, vec->capacity * sizeof(int));
    }
    vec->data[vec->size++] = value;
}

void vector_destroy(Vector *vec) {
    free(vec->data);
    free(vec);
}
