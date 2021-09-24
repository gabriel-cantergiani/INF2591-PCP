#include <stdio.h>
#include <stdlib.h>
#include "task.h"

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

double parallel_task_omp (taskArgs * args) {

    double height = args->right - args->left;
    double totalArea = ( (args->func_ptr(args->left) + args->func_ptr(args->right)) * height )/ 2;

    double area = calculateAreaRecursively(args->left, args->right, args->func_ptr, totalArea, args->tolerance);
    printf("\n[Thread %d] Subinterval area: %f\n", args->thread_id, area);

    return area;
}

void * parallel_task_pthread (void * arg) {

    taskArgs * args = (taskArgs *) arg;

    double height = args->right - args->left;
    double totalArea = ( (args->func_ptr(args->left) + args->func_ptr(args->right)) * height )/ 2;

    double area = calculateAreaRecursively(args->left, args->right, args->func_ptr, totalArea, args->tolerance);
    printf("\n[Thread %d] Subinterval area: %f\n", args->thread_id, area);

    double * result = malloc(sizeof(double));
    *result = area;

    return (void*) result;
}