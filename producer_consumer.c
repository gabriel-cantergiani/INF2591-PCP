#include <stdio.h>
#include "buffer.h"
#include "producer_consumer.h"

void * consume (void * arg) {

    consumerArgs * args = (consumerArgs *) arg;

    for(int i=0; i<args->items_num; i++)
        consome(args->buffer, args->consumerID);

    return NULL;
}

void * produce (void * arg) {

    producerArgs * args = (producerArgs *) arg;

    for(int i=0; i<args->items_num; i++)
        deposita(args->buffer, args->items[i]);

    return NULL;
}