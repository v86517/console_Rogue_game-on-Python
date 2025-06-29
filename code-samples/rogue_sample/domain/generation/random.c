#include "random.h"

int get_random_in_range(int min, int max)
{
    int range_len = max - min + 1;
    return rand() % range_len + min;
}

void swap(void *first, void *second, size_t size_of_elem)
{
    unsigned char *first_elem = first;
    unsigned char *second_elem = second;
    for (size_t i = 0; i < size_of_elem; i++)
    {
        unsigned char temp = first_elem[i];
        first_elem[i] = second_elem[i];
        second_elem[i] = temp;
    }
}

void shuffle_array(void *array, size_t size, size_t size_of_elem)
{
    unsigned char *array_char = array;
    for (size_t i = 0; i < size; i++)
    {
        size_t next = rand() % size;
        unsigned char *first_elem = array_char + i * size_of_elem;
        unsigned char *second_elem = array_char + next * size_of_elem;
        swap(first_elem, second_elem, size_of_elem);
    }
}

