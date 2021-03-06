#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "task.h"
#include "utils.h"

int main (int argc, char * argv[]) {

    if (argc < 3) {
        printf("Usage: ./var1_pthread <num_threads> <tolerance>\n");
        exit(1);
    }

    clock_t begin = clock();

    // Initialize variables
    int num_threads = atoi(argv[1]);
    double tolerance = atof(argv[2]);
    pthread_t worker_threads[num_threads];

    double (*func)(double) = get_func();
    double leftLimit = get_left_limit();
    double rightLimit = get_right_limit();

    // Prepare values
    double height = (rightLimit - leftLimit)/num_threads;

    // Create worker threads
    for (int i = 0; i < num_threads; i++) {

        double left = leftLimit + height*i;
        double right = left + height;

        int thread_id = i + 1;
        taskArgs * args = (taskArgs *) malloc(sizeof(taskArgs));
        args->thread_id = thread_id;
        args->func_ptr = func;
        args->left = left;
        args->right = right;
        args->tolerance = tolerance;
        
        pthread_t p;
        pthread_create(&p, NULL, parallel_task_pthread, args);
        
        worker_threads[i] = p;
        // printf("[Main thread] Worker thread %d created\n", thread_id);
    }

    double totalArea = 0;

    // Wait for worker threads to finish
    for (int i = 0; i < num_threads; i++) {
        void * result;
        pthread_join(worker_threads[i], &result);
        // printf("[Main thread] Worker thread %d finished with subinterval area of: %f\n", i+1, *(double*)result);
        totalArea += *(double*)result;
    }

    clock_t end = clock();

    printf("\n\n-------\n\n");
    printf("[Main thread][Var1 - Pthread] Tolerance: %.10f\n", tolerance);
    printf("[Main thread][Var1 - Pthread] Left limit: %f , Right Limit: %f\n", leftLimit, rightLimit);
    printf("[Main thread][Var1 - Pthread] Number of threads: %d\n", num_threads);
    printf("[Main thread][Var1 - Pthread] Total area: %f\n", totalArea);
    printf("[Main thread][Var1 - Pthread] Total execution time: %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);
    printf("\n\n-------\n\n");

}
