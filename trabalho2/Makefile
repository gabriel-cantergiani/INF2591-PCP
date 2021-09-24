
compile-v1-pthread:
	gcc -Wall -pthread -o var1_pthread var1_pthread.c

run-v1-pthread:
	./var1_pthread 4 0.00005

all-v1-pthread: compile-v1-pthread run-v1-pthread

compile-v1-openmp:
	gcc -Wall -fopenmp -o var1_openmp var1_openmp.c

run-v1-openmp:
	./var1_openmp 4 0.00005

all-v1-openmp: compile-v1-openmp run-v1-openmp