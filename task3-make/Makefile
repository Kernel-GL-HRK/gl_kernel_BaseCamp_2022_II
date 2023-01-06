CC=gcc
CFLAGS=#-Wall -g
TARGET = mains
myPROGRAM=progRegular
flag_so= # -fPIC dynamic library 

flag_lib=0
ifeq ($(flag_lib),1)
    myPROGRAM=progStaticLib
endif




.PHONY: all libs clean

all: ${myPROGRAM}

progRegular: main.o randnum.o
	${CC} ${CFLAGS} $^ -o ${TARGET}

# Static library
progStaticLib: main.o libmylib.a 
	${CC}  -L. -o ${TARGET} main.o -lmylib -lm

main.o: main.c main.h
	${CC} ${CFLAGS} -c $^

randnum.o: randnum.c randnum.h
	${CC} ${CFLAGS} -c $^

libmylib.a: randnum.o
	ar rcs libmylib.a $^

libs: libmylib.a

clean:
	rm -f ${TARGET} *.o *.a *.gch