

typedef struct {
    int consumerID;
    tbuffer * buffer;
    int items_num;
} consumerArgs;

typedef struct {
    int producerID;
    int * items;
    int items_num;
    tbuffer * buffer;
} producerArgs;

void *consume (void * arg);

void *produce (void * arg);