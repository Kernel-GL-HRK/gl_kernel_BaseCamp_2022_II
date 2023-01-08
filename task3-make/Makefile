CC=gcc

.PHONY: all
all: game

game: main.o myrandom.o
	$(CC) $^ -o $@

main.o: main.c myrandom.h
	$(CC) -c $<

myrandom.o: myrandom.c myrandom.h
	$(CC) -c $<

game_static: main.o libmyrandom.a
	$(CC) $^ -o $@

libmyrandom.a: myrandom.o
	ar rcs $@ $?

game_dynamic: main.o libmyrandom.so
	$(CC) $< -L./ -lmyrandom -Wl,-rpath=./ -o $@

libmyrandom.so: myrandom.c myrandom.h
	$(CC) -fPIC -shared $< -o $@

.PHONY: clean
clean:
	rm *.o *.a *.so game game_static game_dynamic