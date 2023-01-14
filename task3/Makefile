OBJ=GuessANumber

all: ${OBJ}

${OBJ}: main.o utility.o
	${CC} main.o utility.o -o ${OBJ}
utility.o: utility.c
	${CC} ${CFLAGS} utility.c
main.o: main.c
	${CC} ${CFLAGS} main.c
exec:
	./GuessANumber
clean:
	rm -rf ./GuessANumber
	rm -rf *.o
