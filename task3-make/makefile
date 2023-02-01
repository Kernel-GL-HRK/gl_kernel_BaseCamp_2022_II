
main.out: main.o random.o
	gcc $^ -o $@

main.o: main.c
	gcc -c $^

random.o: random.c random.h
	gcc -c $<

.PHONY: run

run: main.out
	./main.out
	rm *.out *.o