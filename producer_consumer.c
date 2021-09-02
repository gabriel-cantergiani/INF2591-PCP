#include <stdio.h>
#include "buffer.h"
#include "producer_consumer.h"

void *createConsumer (void * arg) {

    consumerArgs * args = (consumerArgs *) arg;

    printf("[Thread %d] Teste Consome\n", args->consumerID);

    return NULL;
}

void *createProducer (void * arg) {

    producerArgs * args = (producerArgs *) arg;

    printf("[Thread %d] Teste Produz\n", args->producerID);

    return NULL;
}