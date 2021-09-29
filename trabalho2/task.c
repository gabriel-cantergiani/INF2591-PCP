#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "task.h"

double calculate_area_recursively (double left, double right, double (*func)(double), double totalArea, double tolerance) {

    double height = (right-left)/2;
    double middle = (left+right)/2;
    double leftArea = ( (func(left) + func(middle)) * height )/ 2;
    double rightArea = ( (func(middle) + func(right)) * height )/ 2;
    double error = fabs(totalArea - (leftArea + rightArea));

    printf("\n[Thread] func(%f): %f, func(%f):%f\n", left, func(left), right, func(right));
    printf("\n[Thread] Interval = (%f, %f): TotalArea: %f, leftArea:%f, rightArea: %f\n", left, right, totalArea, leftArea, rightArea);
    printf("\n[Thread] Error: %f, Tolerance: %f\n", error, tolerance);

    if (error < tolerance)
        return totalArea;
    else
        return calculate_area_recursively(left, middle, func, leftArea, tolerance) + calculate_area_recursively(middle, right, func, rightArea, tolerance);
}

double parallel_task_omp (taskArgs * args) {

    double height = args->right - args->left;
    double totalArea = ( (args->func_ptr(args->left) + args->func_ptr(args->right)) * height )/ 2;


    double area = calculate_area_recursively(args->left, args->right, args->func_ptr, totalArea, args->tolerance);
    int id = omp_get_thread_num();
    printf("\n[Thread %d] Subinterval area (%f, %f): %f\n", id, args->left, args->right, area);

    return area;
}

void * parallel_task_pthread (void * arg) {

    taskArgs * args = (taskArgs *) arg;

    double height = args->right - args->left;
    double totalArea = ( (args->func_ptr(args->left) + args->func_ptr(args->right)) * height )/ 2;

    double area = calculate_area_recursively(args->left, args->right, args->func_ptr, totalArea, args->tolerance);
    printf("\n[Thread %d] Subinterval area (%f, %f): %f\n", args->thread_id, args->left, args->right, area);

    double * result = malloc(sizeof(double));
    *result = area;

    return (void*) result;
}

void InsereTarefa(taskArgs * args) {
    enQueue(tasksQueue, (void *) args);
}

taskArgs * RetiraTarefa() {
    taskArgs * task = deQueue(tasksQueue);

    return task;
}

void executeTask(taskArgs * args) {

    int id = omp_get_thread_num();
    taskArgs * task;

    if (args == NULL){
        #pragma omp critical
        task = RetiraTarefa();
        if (task == NULL)
            return;
    }
    else
        task = args;

    double left = task->left;
    double right = task->right;
    double (*func)(double) = task->func_ptr;
    double height = (right-left)/2;
    double middle = (left+right)/2;
    double totalArea = ( (func(left) + func(right)) * (right - left) )/ 2;
    double leftArea = ( (func(left) + func(middle)) * height )/ 2;
    double rightArea = ( (func(middle) + func(right)) * height )/ 2;
    double error = fabs(totalArea - (leftArea + rightArea));

    if (error < task->tolerance){
        printf("\n[Thread %d] Task done with error below tolerance. Subinterval area (%f, %f): %f\n", id, left, right, totalArea);
        totalAreaSum += totalArea;
    }
    else {
        taskArgs * newTaskArgsLeft = (taskArgs *) malloc(sizeof(taskArgs));
        newTaskArgsLeft->thread_id = id + 10;
        newTaskArgsLeft->func_ptr = func;
        newTaskArgsLeft->left = left;
        newTaskArgsLeft->right = middle;
        newTaskArgsLeft->tolerance = task->tolerance;
        #pragma omp critical
        InsereTarefa(newTaskArgsLeft);

        taskArgs * newTaskArgsRight = (taskArgs *) malloc(sizeof(taskArgs));
        newTaskArgsRight->thread_id = task->thread_id + 10;
        newTaskArgsRight->func_ptr = func;
        newTaskArgsRight->left = middle;
        newTaskArgsRight->right = right;
        newTaskArgsRight->tolerance = task->tolerance;
        executeTask(newTaskArgsRight);
    }

    executeTask(NULL);
}