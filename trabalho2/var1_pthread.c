#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int quadrado (int x) {
    return x*x;
}

typedef struct {
    int param;
    int (*func_ptr) (int);
} taskArgs;

void * task (void * arg) {

    taskArgs * args = (taskArgs *) arg;

    int result = args->func_ptr(args->param);   
    printf("Result: %d\n", result);

    return NULL;
}

int main (int argc, char * argv[]) {

    if (argc < 3) {
        printf("Usage: ./var1_pthread <num_threads> <tolerance>\n");
        exit(1);
    }

    // Initialize variables
    int num_threads = atoi(argv[1]);
    float tolerance = atof(argv[2]);

    int (*func)(int) = &quadrado;
    taskArgs thread_args[num_threads];
    pthread_t worker_threads[num_threads];

    // Create worker threads
    for (int i = 0; i < num_threads; i++) {

        int thread_id = i + 1;
        taskArgs args = { thread_id, func };
        thread_args[i] = args;
        
        pthread_t p;
        pthread_create(&p, NULL, task, &thread_args[i]);
        worker_threads[i] = p;
        printf("Worker thread %d created\n", thread_id);
    }

    // Wait for worker threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(worker_threads[i], NULL);
        printf("Worker thread %d finished\n", i + 1);
    }

}
