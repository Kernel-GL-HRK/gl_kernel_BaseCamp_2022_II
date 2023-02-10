CC=gcc
OUTPUT=build/guess_a_number
FILEPATH=../task1-simple-program

c_lib=guess_a_number.o randomeGenerate.a
c_dlib=guess_a_number_dll.c 
c_tfile=guess_a_number.o randomeGenerate.o

flag_lib=.
ifeq ($(flag_lib), 1)
req=$(c_tfile)
endif
ifeq ($(flag_lib), 2)
req=$(c_dlib)
else
req=$(c_lib)
endif

target:
	mkdir build/

.PHONY: all

all:${req}
	${CC} $^ -o ${OUTPUT}
guess_a_number.o: 
	${CC} -Wall -c ${FILEPATH}/guess_a_number.c
randomeGenerate.o:
	${CC} -Wall -c ${FILEPATH}/randomeGenerate.c	
randomeGenerate.a: 
	${CC} -Wall -fPIC -c ${FILEPATH}/randomeGenerate.c
	ar rcs randomeGenerate.a randomeGenerate.o
randomeGenerate.so:randomeGenerate_dll.o
	gcc -shared -o librandomeGenerate.so randomeGenerate.o -lc
randomeGenerate_dll.o:
	gcc -fPIC -c ${FILEPATH}/randomeGenerate.c	
guess_a_number_dll.o:randomeGenerate.so
	gcc guess_a_number_dll.c -ldl -o guess_a_number_dll.o
check:
	cppcheck ${req}
