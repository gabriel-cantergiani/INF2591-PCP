#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include "task.h"

#define a 5.0
#define b 12.0

double quadrado (double x) {
    // FAKE PROCESSING...
    fake_processing(x);
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
    int num_threads = atoi(argv[1]);
    double tolerance = atof(argv[2]);
    double totalArea = 0;

    // DEFINICAO DA FUNCAO TESTADA
    double (*func)(double) = &quadrado;

    // Prepare values
    double height = (b-a)/num_threads;
    omp_set_num_threads(num_threads);

    // Create worker threads
    #pragma omp parallel for reduction (+: totalArea)
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

        double result = parallel_task_omp(&args);

        totalArea += result;   
    }

    end = time(NULL);

    printf("\n\n-------\n\n");
    printf("[Main thread][Var1 - OpenMP] Tolerance: %f\n", tolerance);
    printf("[Main thread][Var1 - OpenMP] Number of threads: %d\n", num_threads);
    printf("[Main thread][Var1 - OpenMP] Total area: %f\n", totalArea);
    printf("[Main thread][Var1 - OpenMP] Total execution time: %f seconds\n\n", difftime(end, begin));
    printf("\n\n-------\n\n");

}
