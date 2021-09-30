#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "task.h"
#include "utils.h"

int main (int argc, char * argv[]) {

    if (argc < 4) {
        printf("Usage: ./var1_openmp <num_threads> <tolerance> <num_initial_tasks>\n");
        exit(1);
    }

    time_t begin, end;
     
    begin = time(NULL);

    // Initializevariables
    int total_num_threads = atoi(argv[1]);
    double tolerance = atof(argv[2]);
    int initial_num_tasks = atoi(argv[3]);

    double (*func)(double) = get_func();
    double leftLimit = get_left_limit();
    double rightLimit = get_right_limit();

    // Initialize global variables
    totalAreaSum = 0;
    tasksQueue = createQueue();

    if (initial_num_tasks < total_num_threads)
        initial_num_tasks = total_num_threads;

    // Prepare values
    double height = (rightLimit-leftLimit)/initial_num_tasks;

    // Create initial tasks
    for (int i = 0; i < initial_num_tasks; i++) {

        double left = leftLimit + height*i;
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

    omp_set_num_threads(total_num_threads);

    // Start worker threads
    #pragma omp parallel for num_threads(total_num_threads) reduction (+: totalAreaSum)
    for (int i = 0; i < total_num_threads; i++) {
        executeTask(NULL);
    }

    end = time(NULL);

    printf("\n\n-------\n\n");
    printf("[Main thread][Var2 - OpenMP] Tolerance: %.10f\n", tolerance);
    printf("[Main thread][Var2 - OpenMP] Left limit: %f , Right Limit: %f\n", leftLimit, rightLimit);
    printf("[Main thread][Var2 - OpenMP] Initial number of tasks: %d\n", initial_num_tasks);
    printf("[Main thread][Var2 - OpenMP] Number of threads: %d\n", total_num_threads);
    printf("[Main thread][Var2 - OpenMP] Total area: %f\n", totalAreaSum);
    printf("[Main thread][Var2 - OpenMP] Total execution time: %f seconds", difftime(end, begin));
    printf("\n\n-------\n\n");

}
