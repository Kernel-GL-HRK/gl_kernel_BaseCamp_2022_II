CC=gcc
CFLAGS=

.PHONY: all
all: game

.PHONY: debug
debug: CFLAGS+=-g
debug: game

.PHONY: all_warnings
all_warnings:CFLAGS+=-Wall -Werror -Wextra
all_warnings:game

game: main.o myrandom.o
	$(CC) $^ -o $@

main.o: main.c myrandom.h
	$(CC) $(CFLAGS) -c $<

myrandom.o: myrandom.c myrandom.h
	$(CC) $(CFLAGS) -c $<

game_static: main.o libmyrandom.a
	$(CC) $^ -o $@

libmyrandom.a: myrandom.o
	ar rcs $@ $?

game_dynamic: main.o libmyrandom.so
	$(CC) $< -L./ -lmyrandom -Wl,-rpath=./ -o $@

libmyrandom.so: myrandom.c myrandom.h
	$(CC) $(CFLAGS) -fPIC -shared $< -o $@

.PHONY: cppcheck
cppcheck:
	cppcheck --enable=all *.c *.h

.PHONY: checkpatch
checkpatch: checkpatch.pl
	./checkpatch.pl --no-tree -f *.c *.h

.PHONY: clean
clean:
	rm *.o *.a *.so game game_static game_dynamic