CC=gcc

main.out: main.o random.o
	${CC} $^ -g -o $@

main.o: main.c
	${CC} -c $^

random.o: random.c random.h
	${CC} -c $<

.PHONY: run

run: main.out
	./main.out
	rm *.out *.o