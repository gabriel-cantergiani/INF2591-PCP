#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include <pthread.h>

void *createConsumer (void * arg) {

    int thread_id = * (int *) arg;

    // printf("[Thread %d] Teste Consumo: %d\n", thread_id, consome(buf, 0));
    printf("[Thread %d] Teste Consome\n", thread_id);

    return NULL;
}

void *createProducer (void * arg) {

    int thread_id = * (int *) arg;

    printf("[Thread %d] Teste Produz\n", thread_id);

    return NULL;
}

int main(int argc, char * argv[]) {

    if (argc < 5) {
        printf("Usage: ./main <buffer_size> <number_of_producers> <number_of_consumers> <number_of_items>");
        exit(1);
    }

    // Initialize variables
    int N = atoi(argv[1]);
    int P = atoi(argv[2]);
    int C = atoi(argv[3]);
    int I = atoi(argv[4]);

    pthread_t producer_threads[P];
    pthread_t consumer_threads[C];
    int producer_threads_ids[P];
    int consumer_threads_ids[C];
    int items[I];

    for (int i=0; i < I; i++)
        items[i] = i;

    // Create buffer
    tbuffer * buffer = iniciabuffer(N, P, C);

    // Create consumer threads
    for (int i = 0; i < C; i++) {
        pthread_t p;
        consumer_threads[i] = p;
        consumer_threads_ids[i] = i + 1;
        pthread_create(&p, NULL, createConsumer, &consumer_threads_ids[i]);
        printf("Thread %d created\n", consumer_threads_ids[i]);
    }

    // Create producer threads
    for (int i = 0; i < P; i++) {
        pthread_t p;
        producer_threads[i] = p;
        producer_threads_ids[i] = C + i + 1;
        pthread_create(&p, NULL, createProducer, &producer_threads_ids[i]);
        printf("Thread %d created\n", producer_threads_ids[i]);
    }

    // Wait for consumer threads to finish
    for (int i = 0; i < C; i++) {
        pthread_join(consumer_threads[i], NULL);
        printf("Thread %d finished\n", consumer_threads_ids[i]);
    }

    // Wait for producer threads to finish
    for (int i = 0; i < P; i++) {
        pthread_join(producer_threads[i], NULL);
        printf("Thread %d finished\n", producer_threads_ids[i]);
    }

    return 0;
}