OBJ=GuessANumber

all: ${OBJ}

all_static_lib: main.o static_libs
	${CC} -lm -o ${OBJ} main.o -L. -lutility

libutility.a: utility.o
	ar rcs libutility.a utility.o

static_libs: libutility.a

${OBJ}: main.o utility.o
	${CC} main.o utility.o -o ${OBJ}

utility.o: utility.c
	${CC} ${CFLAGS} ${DEBUGFLAGS} -c utility.c

main.o: main.c
	${CC} ${CFLAGS} ${DEBUGFLAGS} -c main.c

exec:
	./GuessANumber

clean:
	rm -rf ./GuessANumber
	rm -rf *.o
	rm -rf *.a
