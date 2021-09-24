#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "task.h"

#define a 5.0
#define b 12.0

double quadrado (double x) {
    return x*x;
}

int main (int argc, char * argv[]) {

    if (argc < 3) {
        printf("Usage: ./var1_openmp <num_threads> <tolerance>\n");
        exit(1);
    }

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

    printf("\n-------\n");
    printf("[Main thread] Total area: %f\n", totalArea);

}
