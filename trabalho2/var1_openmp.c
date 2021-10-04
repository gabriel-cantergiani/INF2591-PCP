#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "task.h"
#include "utils.h"

int main (int argc, char * argv[]) {

    if (argc < 3) {
        printf("Usage: ./var1_openmp <num_threads> <tolerance>\n");
        exit(1);
    }

    clock_t begin = clock();

    // Initialize variables
    int num_threads = atoi(argv[1]);
    double tolerance = atof(argv[2]);
    double totalArea = 0;

    double (*func)(double) = get_func();
    double leftLimit = get_left_limit();
    double rightLimit = get_right_limit();

    // Prepare values
    double height = (rightLimit-leftLimit)/num_threads;
    omp_set_num_threads(num_threads);

    // Create worker threads
    #pragma omp parallel for reduction (+: totalArea)
    for (int i = 0; i < num_threads; i++) {

        double left = leftLimit + height*i;
        double right = left + height;

        int thread_id = i + 1;
        taskArgs args = { 
            thread_id,
            func,
            left,
            right,
            tolerance
        };

        double result = parallel_task_omp(&args);

        totalArea += result;   
    }

    clock_t end = clock();

    printf("\n\n-------\n\n");
    printf("[Main thread][Var1 - OpenMP] Tolerance: %.10f\n", tolerance);
    printf("[Main thread][Var1 - OpenMP] Left limit: %f , Right Limit: %f\n", leftLimit, rightLimit);
    printf("[Main thread][Var1 - OpenMP] Number of threads: %d\n", num_threads);
    printf("[Main thread][Var1 - OpenMP] Total area: %f\n", totalArea);
    printf("[Main thread][Var1 - OpenMP] Total execution time: %f seconds\n\n", (double)(end - begin) / CLOCKS_PER_SEC);
    printf("\n\n-------\n\n");

}
