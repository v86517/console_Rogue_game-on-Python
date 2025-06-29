#include "queue.h"

queue* create_queue(void)
{
    queue *Queue = malloc(sizeof(queue));
    Queue->begin = Queue->end = NULL;
    return Queue;
}

node* create_node(const object_t *coords)
{
    node *Node = malloc(sizeof(node));
    Node->coords = *coords;
    Node->next = NULL;
    return Node;
}

bool is_empty(queue *q)
{
    return q->begin == NULL;
}

void enqueue(queue *q, const object_t *coords)
{
    node* Node = create_node(coords);
    
    if (q->end == NULL)
        q->begin = q->end = Node;
    else
    {
        q->end->next = Node;
        q->end = Node;
    }
}

object_t dequeue(queue *q)
{
    node *temp = q->begin;
    q->begin = q->begin->next;
    if (q->begin == NULL)
        q->end = NULL;
    object_t coords = temp->coords;
    free(temp);
    return coords;
}

void clear_queue(queue *q)
{
    while (!is_empty(q))
        (void) dequeue(q);
    free(q);
}
