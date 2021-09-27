

typedef struct {
    int thread_id;
    double (*func_ptr) (double);
    double left;
    double right;
    double tolerance;
} taskArgs;

taskArgs ** tasksQueue;
int nextReadIndex;
int nextWriteIndex;
int queueMaxSize;
double totalAreaSum;

double parallel_task_omp (taskArgs * args);

void * parallel_task_pthread (void * arg);

void fake_processing(double x);

void InsereTarefa(taskArgs * args);

void executeTask(taskArgs * args);
