CC=gcc
CFLAGS=#-Wall -g
main_n = mains
myPROGRAM=progRegular
flag_so= # -fPIC dynamic library 

.PHONY: all clean

all: ${myPROGRAM}

progRegular: main.o randnum.o
	${CC} ${CFLAGS} $^ -o ${main_n}

main.o: main.c main.h
	${CC} ${CFLAGS} -c $^

randnum.o: randnum.c randnum.h
	${CC} ${CFLAGS} -c $^

clean:
	rm -f ${main_n} *.o *.gch