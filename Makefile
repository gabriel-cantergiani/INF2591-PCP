
compile:
	gcc -o main main.c buffer.c producer_consumer.c

run:
	./main

all: compile run