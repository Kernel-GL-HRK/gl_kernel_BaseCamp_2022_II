CC=gcc

.PHONY: all clean debug all_warning

NAME = guess_game

FILE = *.c *h

FLAG =

debug: FLAG = -g
debug:all

all_warning:FLAG = -Wall
all_warning:all


all: main.o random.o
	${CC} $^ -o ${NAME}

main.o: guess_game.c random.h
	${CC} -c ${FLAG}  $< -o $@

random.o: random.c random.h
	${CC} -c ${FLAG} $<

clean:
	rm *.o ${NAME} *.a

#static lib

game_with_lib: main.o librandom.a
	${CC} $^ ${FLAG} -o ${NAME}

librandom.a: random.o 
	ar rcs $@ $^ 

check:
	../script/checkpatch.pl --no-tree -f ${FILE}
