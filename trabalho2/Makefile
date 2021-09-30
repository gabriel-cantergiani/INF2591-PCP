
#######################

# Variant 1 - Pthreads

compile-v1-pthread:
	gcc -Wall -pthread -fopenmp -o var1_pthread var1_pthread.c task.c queue.c utils.c -lm

run-v1-pthread-1:
	./var1_pthread 1 0.00001

run-v1-pthread-2:
	./var1_pthread 2 0.00001

run-v1-pthread-3:
	./var1_pthread 3 0.00001

run-v1-pthread-4:
	./var1_pthread 4 0.00001

run-v1-pthread-5:
	./var1_pthread 1 0.00000001

run-v1-pthread-6:
	./var1_pthread 2 0.00000001

run-v1-pthread-7:
	./var1_pthread 3 0.00000001

run-v1-pthread-8:
	./var1_pthread 4 0.00000001

all-v1-pthread: compile-v1-pthread run-v1-pthread-1 run-v1-pthread-2 run-v1-pthread-3 run-v1-pthread-4 run-v1-pthread-5 run-v1-pthread-6 run-v1-pthread-7 run-v1-pthread-8

#######################

# Variant 1 - OpenMP

compile-v1-openmp:
	gcc -Wall -fopenmp -o var1_openmp var1_openmp.c task.c queue.c utils.c -lm

run-v1-openmp-1:
	./var1_openmp 1 0.0000001

run-v1-openmp-2:
	./var1_openmp 2 0.0000001

run-v1-openmp-3:
	./var1_openmp 3 0.0000001

run-v1-openmp-4:
	./var1_openmp 4 0.0000001

run-v1-openmp-5:
	./var1_openmp 1 0.0000000001

run-v1-openmp-6:
	./var1_openmp 2 0.0000000001

run-v1-openmp-7:
	./var1_openmp 3 0.0000000001

run-v1-openmp-8:
	./var1_openmp 4 0.0000000001

all-v1-openmp: compile-v1-openmp run-v1-openmp-1 run-v1-openmp-2 run-v1-openmp-3 run-v1-openmp-4 run-v1-openmp-5 run-v1-openmp-6 run-v1-openmp-7 run-v1-openmp-8

#######################

# Variant 2 - OpenMP

compile-v2-openmp:
	gcc -Wall -fopenmp -o var2_openmp var2_openmp.c task.c queue.c utils.c -lm

run-v2-openmp-1:
	./var2_openmp 1 0.00001 2

run-v2-openmp-2:
	./var2_openmp 2 0.00001 2

run-v2-openmp-3:
	./var2_openmp 3 0.00001 2

run-v2-openmp-4:
	./var2_openmp 4 0.00001 2

run-v2-openmp-5:
	./var2_openmp 1 0.00000001 2

run-v2-openmp-6:
	./var2_openmp 2 0.00000001 2

run-v2-openmp-7:
	./var2_openmp 3 0.00000001 2

run-v2-openmp-8:
	./var2_openmp 4 0.00000001 2

all-v2-openmp: compile-v2-openmp run-v2-openmp-1 run-v2-openmp-2 run-v2-openmp-3 run-v2-openmp-4 run-v2-openmp-5 run-v2-openmp-6 run-v2-openmp-7 run-v2-openmp-8