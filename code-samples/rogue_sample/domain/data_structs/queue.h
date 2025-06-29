#ifndef QUEUE_H__
#define QUEUE_H__

#include <stdlib.h>
#include <stdbool.h>
#include "entities.h"

typedef struct Node
{
    object_t coords;
    struct Node *next;
} node;

typedef struct Queue
{
    node *begin;
    node *end;
} queue;

queue* create_queue(void);
node* create_node(const object_t *coords);
bool is_empty(queue *q);
void enqueue(queue *q, const object_t *coords);
object_t dequeue(queue *q);
void clear_queue(queue *q);

#endif // QUEUE_H__

