#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define a 5.0
#define b 12.0

double quadrado (double x) {
    return x*x;
}

typedef struct {
    int thread_id;
    double (*func_ptr) (double);
    double left;
    double right;
    double tolerance;
} taskArgs;

double calculateAreaRecursively (double left, double right, double (*func)(double), double totalArea, double tolerance) {

    double height = (right-left)/2;
    double middle = (left+right)/2;
    double leftArea = ( (func(left) + func(middle)) * height )/ 2;
    double rightArea = ( (func(middle) + func(right)) * height )/ 2;
    double error = totalArea - (leftArea + rightArea);

    if (error < tolerance)
        return totalArea;
    else
        return calculateAreaRecursively(left, middle, func, leftArea, tolerance) + calculateAreaRecursively(middle, right, func, rightArea, tolerance);
}

double task (taskArgs * args) {

    double height = args->right - args->left;
    double totalArea = ( (args->func_ptr(args->left) + args->func_ptr(args->right)) * height )/ 2;

    double area = calculateAreaRecursively(args->left, args->right, args->func_ptr, totalArea, args->tolerance);
    printf("\n[Thread %d] Subinterval area: %f\n", args->thread_id, area);

    return area;
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

        double result = task(&args);

        totalArea += result;   
    }

    printf("\n-------\n");
    printf("[Main thread] Total area: %f\n", totalArea);

}
