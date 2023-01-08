CC=gcc

.PHONY: all
all: game

game: main.o myrandom.o
	$(CC) $^ -o $@

main.o: main.c myrandom.h
	$(CC) -c $<

myrandom.o: myrandom.c myrandom.h
	$(CC) -c $<

.PHONY: clean
clean:
	rm *.o game