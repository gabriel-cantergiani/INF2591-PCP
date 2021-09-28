# INF2591-PCP

# Adaptive Quadrature Algorithm

This project implements 3 variants of the adaptive quadrature algorithm for calculating the area under a function, in a given interval. All variants use multithreading techniques and OpenMP or Pthread libraries:

 - Variant 1-pthread: each thread calculates the area for a given subinterval using pthread.

 - Variant 1-openmp: each thread calculates the area for a given subinterval using OpenMP.

 - Variant 2: multiple tasks are initially created, and new subtasks are inserted in a queue. Multiple threads execute tasks from the queue using OpenMP.

Instructions to run:

- To compile variant 1-pthread using gcc:
```bash
$ make compile-v1-pthread
```

- To run variant 1-pthread using 4 threads and 0.00001 tolerance:
```bash
$ make run-v1-pthread-4
```

- To compile and run all tests for variant 1-pthread:
```bash
$ make all-v1-pthread
```

The same commands are used to compile and run the other variants:

```bash
$ make compile-v1-openmp
```

```bash
$ make run-v1-openmp-4
```

```bash
$ make all-v1-openmp
```

```bash
$ make compile-v2-openmp
```

```bash
$ make run-v2-openmp-4
```

```bash
$ make all-v2-openmp
```