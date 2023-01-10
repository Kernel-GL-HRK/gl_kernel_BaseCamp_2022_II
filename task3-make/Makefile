CC=gcc

.PHONY: all clean

NAME = guess_game

all: main.o random.o
	${CC} $^ -o ${NAME}

main.o: guess_game.c random.h
	${CC} -c -Wall $< -o $@

random.o: random.c random.h
	${CC} -c -Wall $<

clean:
	rm *.o ${NAME}
