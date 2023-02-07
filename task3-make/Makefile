CC=gcc
OUTPUT=build/guess_a_number
FILEPATH=../task1-simple-program

target:
	mkdir build/

all:guess_a_number.o randomeGenerate.o
	${CC} $^ -o ${OUTPUT}
guess_a_number.o: 
	${CC} -Wall -c ${FILEPATH}/guess_a_number.c
randomeGenerate.o:
	${CC} -Wall -c ${FILEPATH}/randomeGenerate.c	
