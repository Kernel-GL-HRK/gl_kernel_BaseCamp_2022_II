OBJ=GuessANumber

all: ${OBJ}

all_static_lib: main.o static_libs
	${CC} -lm -o ${OBJ} main.o -L. -lutility

all_shared_lib: main.o shared_libs
	$(CC) -o ${OBJ} main.o -Wl,-rpath=. -L. -lutility

libutility.a: utility.o
	ar rcs libutility.a utility.o

libutility.so: utility.o
	$(CC) -shared -fPIC utility.o

static_libs: libutility.a

shared_libs: libutility.so

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
