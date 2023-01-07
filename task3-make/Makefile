CC=gcc

all: main.o guess.o
	${CC} $^ -o main

main.o: main.c
	${CC} -Wall -c $<
guess.o: guess.c
	${CC} -Wall -c $<

.PHONY:all

clean:
	rm *.o

