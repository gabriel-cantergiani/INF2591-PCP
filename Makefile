
compile:
	gcc -Wno-deprecated-declarations -o main main.c buffer.c producer_consumer.c

run1:
	./main 5 1 1 3

all: compile run1

all-compose:
	docker-compose up pcp