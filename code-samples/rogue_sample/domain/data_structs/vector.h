#ifndef VECTOR_H__
#define VECTOR_H__

#include "entities.h"
#include <stdlib.h>
#include <stdbool.h>

#define INIT_CAPACITY 4
#define STEP_CAPACITY 2

typedef struct vector
{
    size_t size;
    size_t capacity;
    directions_e *data;
} vector;

vector *create_vector(void);
void push_back(vector *v, directions_e num);
bool is_empty_vector(vector *v);
void reverse_vector(vector *v);
void destroy_vector(vector **v);

#endif // VECTOR_H__
