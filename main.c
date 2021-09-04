#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "buffer.h"
#include "producer_consumer.h"


int main(int argc, char * argv[]) {

    if (argc < 5) {
        printf("Usage: ./main <buffer_size> <number_of_producers> <number_of_consumers> <number_of_items>\n");
        exit(1);
    }

    // Initialize variables
    int N = atoi(argv[1]);
    int P = atoi(argv[2]);
    int C = atoi(argv[3]);
    int I = atoi(argv[4]);

    pthread_t producer_threads[P];
    pthread_t consumer_threads[C];
    producerArgs producer_threads_args[P];
    consumerArgs consumer_threads_args[C];
    int items[I];

    for (int i=0; i < I; i++)
        items[i] = 10+i;

    // Create buffer
    tbuffer * buffer = iniciabuffer(N, P, C);

    // Create consumer threads
    for (int i = 0; i < C; i++) {

        int thread_id = i + 1;
        consumerArgs args = { thread_id, buffer };
        consumer_threads_args[i] = args;

        pthread_t p;
        pthread_create(&p, NULL, createConsumer, &consumer_threads_args[i]);
        consumer_threads[i] = p;

        printf("Consumer thread %d created\n", thread_id);
    }

    // Create producer threads
    for (int i = 0; i < P; i++) {

        int thread_id = C + i + 1;
        producerArgs args = { thread_id, items, buffer };
        producer_threads_args[i] = args;

        pthread_t p;
        pthread_create(&p, NULL, createProducer, &producer_threads_args[i]);
        producer_threads[i] = p;

        printf("Producer thread %d created\n", thread_id);
    }

    // Wait for consumer threads to finish
    for (int i = 0; i < C; i++) {
        pthread_join(consumer_threads[i], NULL);
        printf("Consumer thread %d finished\n", i + 1);
    }

    // Wait for producer threads to finish
    for (int i = 0; i < P; i++) {
        pthread_join(producer_threads[i], NULL);
        printf("Producer thread %d finished\n", C + i + 1);
    }

    finalizabuffer(buffer);

    return 0;
}