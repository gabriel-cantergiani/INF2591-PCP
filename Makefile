
compile:
	gcc -o main main.c buffer.c

run:
	./main

all: compile run