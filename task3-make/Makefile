CC=gcc
CFLAGS=#-Wall -g
TARGET = mains
myPROGRAM=progRegular
flag_so= # -fPIC dynamic library 

flag_lib=0
ifeq ($(flag_lib),1)
    myPROGRAM=progStaticLib
endif
ifeq ($(flag_lib),2)
    myPROGRAM=progDynamicLib
	flag_so=-fPIC 
endif



.PHONY: all libs libs_so clean

all: ${myPROGRAM}

progRegular: main.o randnum.o
	${CC} ${CFLAGS} $^ -o ${TARGET}

# Static library
progStaticLib: main.o libmylib.a 
	${CC}  -L. -o ${TARGET} main.o -lmylib -lm

# Dynamic library
progDynamicLib: main.o libmylib.so 
	${CC} main.o -lmylib -L./ -o ${TARGET}

main.o: main.c main.h
	${CC} ${CFLAGS} -c $^

randnum.o: randnum.c randnum.h
	${CC} ${flag_so} ${CFLAGS} -c $^

libmylib.a: randnum.o
	ar rcs libmylib.a $^

libs: libmylib.a

libmylib.so: randnum.o 
	${CC} -shared -Wl,-soname,libmylib.so -o $@ $^ -lc

libs_so: libmylib.so

clean:
	rm -f ${TARGET} *.o *.a *.so *.gch