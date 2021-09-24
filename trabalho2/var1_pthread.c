#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "task.h"

#define a 5.0
#define b 12.0

double quadrado (double x) {
    return x*x;
}

int main (int argc, char * argv[]) {

    if (argc < 3) {
        printf("Usage: ./var1_pthread <num_threads> <tolerance>\n");
        exit(1);
    }

    // Initialize variables
    int num_threads = atoi(argv[1]);
    double tolerance = atof(argv[2]);

    double (*func)(double) = &quadrado;
    pthread_t worker_threads[num_threads];

    // Prepare values
    double height = (b-a)/num_threads;

    // Create worker threads
    for (int i = 0; i < num_threads; i++) {

        double left = a + height*i;
        double right = left + height;

        int thread_id = i + 1;
        taskArgs args = { 
            thread_id,
            func,
            left,
            right,
            tolerance
        };
        
        pthread_t p;
        pthread_create(&p, NULL, parallel_task_pthread, &args);
        
        worker_threads[i] = p;
        printf("[Main thread] Worker thread %d created\n", thread_id);
    }

    double totalArea = 0;

    // Wait for worker threads to finish
    for (int i = 0; i < num_threads; i++) {
        void * result;
        pthread_join(worker_threads[i], &result);
        printf("[Main thread] Worker thread %d finished with subinterval area of: %f\n", i+1, *(double*)result);
        totalArea += *(double*)result;
    }

    printf("\n-------\n");
    printf("[Main thread] Total area: %f\n", totalArea);

}
