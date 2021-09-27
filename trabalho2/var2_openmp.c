#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "task.h"

#define a 5.0
#define b 12.0
#define NUMINICIAL 2
#define QUEUESIZE 1000
#define FAKE_PROCESSING_LOOP_SIZE 10000000

double quadrado (double x) {
    // FAKE PROCESSING...
    fake_processing(FAKE_PROCESSING_LOOP_SIZE);
    return x*x;
}

int main (int argc, char * argv[]) {

    if (argc < 3) {
        printf("Usage: ./var1_openmp <num_threads> <tolerance>\n");
        exit(1);
    }

    time_t begin, end;
     
    begin = time(NULL);

    // Initialize variables
    int total_num_threads = atoi(argv[1]);
    double tolerance = atof(argv[2]);
    // double totalArea = 0;

    // DEFINICAO DA FUNCAO TESTADA
    double (*func)(double) = &quadrado;

    // Prepare values
    double height = (b-a)/NUMINICIAL;
    omp_set_num_threads(NUMINICIAL);

    // Initialize global variables
    totalAreaSum = 0;
    tasksQueue = (taskArgs **) malloc(sizeof(taskArgs *) * QUEUESIZE);
    nextReadIndex = 0;
    nextWriteIndex = 0;
    queueMaxSize = QUEUESIZE - 1;

    for (int i = 0; i < QUEUESIZE; i++)
        tasksQueue[i] = NULL;

    // Create initial worker threads
    for (int i = 0; i < NUMINICIAL; i++) {

        double left = a + height*i;
        double right = left + height;

        int thread_id = i + 1;
        taskArgs * newTaskArgs = (taskArgs *) malloc(sizeof(taskArgs));
        newTaskArgs->thread_id = thread_id;
        newTaskArgs->func_ptr = func;
        newTaskArgs->left = left;
        newTaskArgs->right = right;
        newTaskArgs->tolerance = tolerance;
        InsereTarefa(newTaskArgs);
    }

    #pragma omp parallel for num_threads(total_num_threads) reduction (+: totalAreaSum)
    for (int i = 0; i < total_num_threads; i++) {
        executeTask(NULL);
    }

    end = time(NULL);

    printf("\n\n-------\n\n");
    printf("[Main thread][Var2 - OpenMP] Tolerance: %f\n", tolerance);
    printf("[Main thread][Var2 - OpenMP] Initial number of tasks: %d\n", NUMINICIAL);
    printf("[Main thread][Var2 - OpenMP] Number of threads: %d\n", total_num_threads);
    printf("[Main thread][Var2 - OpenMP] Total area: %f\n", totalAreaSum);
    printf("[Main thread][Var2 - OpenMP] Total execution time: %f seconds\n\n", difftime(end, begin));
    printf("\n\n-------\n\n");

}
