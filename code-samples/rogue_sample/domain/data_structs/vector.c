#include "vector.h"

vector *create_vector(void)
{
    vector *v = (vector *)malloc(sizeof(vector));
    v->size = 0;
    v->capacity = INIT_CAPACITY;
    v->data = (directions_e *)malloc(v->capacity * sizeof(directions_e));
    return v;
}

static void expand_vector(vector *v)
{
    v->capacity *= STEP_CAPACITY;
    v->data = (directions_e *)realloc(v->data, v->capacity * sizeof(directions_e));
}

void push_back(vector *v, directions_e dir)
{
    if (v->capacity == v->size)
        expand_vector(v);
    v->data[v->size] = dir;
    v->size++;
}

bool is_empty_vector(vector *v)
{
    return v->size == 0;
}

void reverse_vector(vector *v)
{
    for (size_t i = 0; i < v->size / 2; i++)
    {
        directions_e temp = v->data[i];
        v->data[i] = v->data[v->size - i - 1];
        v->data[v->size - i - 1] = temp; 
    }
}

void destroy_vector(vector **v)
{
    if (*v)
    {
        free((*v)->data);
        free(*v);
    }
    *v = NULL;
}
