
typedef struct {
    int thread_id;
    double (*func_ptr) (double);
    double left;
    double right;
    double tolerance;
} taskArgs;


double parallel_task_omp (taskArgs * args);

void * parallel_task_pthread (void * arg);