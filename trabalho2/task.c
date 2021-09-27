#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "task.h"

void fake_processing(double x) {
    for (int i=0; i< x; i++) {
        double k = x*x*x;
        double p = k*500.50005;
        double m = p/(x*x);
        k = m;
    }
}

double calculate_area_recursively (double left, double right, double (*func)(double), double totalArea, double tolerance) {

    // int id = omp_get_thread_num();
    double height = (right-left)/2;
    double middle = (left+right)/2;
    double leftArea = ( (func(left) + func(middle)) * height )/ 2;
    double rightArea = ( (func(middle) + func(right)) * height )/ 2;
    double error = totalArea - (leftArea + rightArea);

    // printf("\n[Thread %d] TotalArea=%f, leftArea=%f, rightArea=%f\n", id, totalArea, leftArea, rightArea);
    // printf("\n[Thread %d] Error=%f, tolerance=%f\n", id, error, tolerance);
    if (error < tolerance)
        return totalArea;
    else{
        // printf("\n[Thread %d] Calling recursively...left=%f, middle=%f\n", id, left, middle);
        // printf("\n[Thread %d] Calling recursively...middle=%f, right=%f\n", id, middle, right);
        return calculate_area_recursively(left, middle, func, leftArea, tolerance) + calculate_area_recursively(middle, right, func, rightArea, tolerance);
    }
}

double parallel_task_omp (taskArgs * args) {

    // int id = omp_get_thread_num();
    // printf("\n[Thread %d] executing task... Left: %f, Right: %f\n", id, args->left, args->right);
    double height = args->right - args->left;
    double totalArea = ( (args->func_ptr(args->left) + args->func_ptr(args->right)) * height )/ 2;
    // printf("\n[Thread %d] TotalArea=%f, func(left)=%f, func(right)=%f, fullHeight=%f\n", id, totalArea,  func(left), func(right), fullHeight);


    double area = calculate_area_recursively(args->left, args->right, args->func_ptr, totalArea, args->tolerance);
    printf("\n[Thread %d] Subinterval area: %f\n", args->thread_id, area);

    return area;
}

void * parallel_task_pthread (void * arg) {

    taskArgs * args = (taskArgs *) arg;

    double height = args->right - args->left;
    double totalArea = ( (args->func_ptr(args->left) + args->func_ptr(args->right)) * height )/ 2;

    double area = calculate_area_recursively(args->left, args->right, args->func_ptr, totalArea, args->tolerance);
    printf("\n[Thread %d] Subinterval area: %f\n", args->thread_id, area);

    double * result = malloc(sizeof(double));
    *result = area;

    return (void*) result;
}

void InsereTarefa(taskArgs * args) {

    // int id = omp_get_thread_num();
    if (tasksQueue[nextWriteIndex] != NULL) {
        // printf("\n[Thread %d] ERROR: Could not insert task in queue position: %d\n", id, nextWriteIndex);
        return;
    }

    // printf("\n[Thread %d] Inserting task in queue position: %d\n", id, nextWriteIndex);
    tasksQueue[nextWriteIndex] = args;
    nextWriteIndex = (nextWriteIndex + 1) % queueMaxSize;

}

taskArgs * RetiraTarefa() {

    // int id = omp_get_thread_num();

    taskArgs * task;

    task = tasksQueue[nextReadIndex];

    // printf("\n[Thread %d] Retreaving task from queue position: %d\n", id, nextReadIndex);
    tasksQueue[nextReadIndex] = NULL;
    nextReadIndex = (nextReadIndex + 1) % queueMaxSize;


    return task;

}

void executeTask(taskArgs * args) {

    int id = omp_get_thread_num();
    taskArgs * task;
    if (args == NULL){
        // printf("\n[Thread %d] No task received. Retrieving from queue...\n", id);
        #pragma omp critical
        task = RetiraTarefa();
        if (task == NULL){
            // printf("\n[Thread %d] ERROR Unable to retrieve trask from queue...\n", id);
            return;
        }
    }
    else
        task = args;

    // printf("\n[Thread %d] executing task... Left: %f, Right: %f\n", id, task->left, task->right);
    double left = task->left;
    double right = task->right;
    double (*func)(double) = task->func_ptr;
    double height = (right-left)/2;
    double middle = (left+right)/2;
    double totalArea = ( (func(left) + func(right)) * (right - left) )/ 2;
    double leftArea = ( (func(left) + func(middle)) * height )/ 2;
    double rightArea = ( (func(middle) + func(right)) * height )/ 2;
    double error = totalArea - (leftArea + rightArea);

    // printf("\n[Thread %d] TotalArea=%f, func(left)=%f, func(right)=%f, fullHeight=%f\n", id, totalArea,  func(left), func(right), fullHeight);
    // printf("\n[Thread %d] TotalArea=%f, leftArea=%f, rightArea=%f\n", id, totalArea, leftArea, rightArea);
    // printf("\n[Thread %d] Error=%f, tolerance=%f\n", id, error, task->tolerance);

    if (error < task->tolerance){
        printf("\n[Thread %d] Task done with error below tolerance. Adding to totalArea: %f\n", id, totalArea);
        totalAreaSum += totalArea;
    }
    else {
        // printf("\n[Thread %d] Task done. Creating new task and pushing to queue...(left=%f, right=%f)\n", id, left, middle);
        taskArgs * newTaskArgsLeft = (taskArgs *) malloc(sizeof(taskArgs));
        newTaskArgsLeft->thread_id = id + 10;
        newTaskArgsLeft->func_ptr = func;
        newTaskArgsLeft->left = left;
        newTaskArgsLeft->right = middle;
        newTaskArgsLeft->tolerance = task->tolerance;
        #pragma omp critical
        InsereTarefa(newTaskArgsLeft);

        // printf("\n[Thread %d] Creating new task and executing it...(left=%f, right=%f)\n", task->thread_id, middle, right);
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