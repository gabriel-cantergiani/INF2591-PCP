

typedef struct {
    int consumerID;
    tbuffer * buffer;
} consumerArgs;

typedef struct {
    int producerID;
    int * items;
    tbuffer * buffer;
} producerArgs;

void *createConsumer (void * arg);

void *createProducer (void * arg);