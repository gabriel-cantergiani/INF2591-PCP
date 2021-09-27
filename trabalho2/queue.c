#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
 
struct Node {
    void * item;
    struct Node* next;
};
 
struct Node* createNode(void * item)
{
    struct Node* temp = (struct Node*) malloc(sizeof(struct Node));
    temp->item = item;
    temp->next = NULL;
    return temp;
}
 
Queue* createQueue()
{
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue->out = queue->in = NULL;
    return queue;
}
 
void enQueue(Queue* q, void * item)
{
    struct Node* temp = createNode(item);
 
    if (q->in == NULL) {
        q->out = q->in = temp;
        return;
    }
 
    q->in->next = temp;
    q->in = temp;
}
 
void * deQueue(Queue* q)
{
    if (q->out == NULL)
        return NULL;
 
    struct Node* temp = q->out;
 
    q->out = q->out->next;
 
    if (q->out == NULL)
        q->in = NULL;
 
    return temp->item;
}