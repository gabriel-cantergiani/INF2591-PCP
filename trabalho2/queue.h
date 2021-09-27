
typedef struct {
    struct Node *out, *in;
} Queue;

Queue* createQueue();

void enQueue(Queue* q, void * item);

void * deQueue(Queue* q);