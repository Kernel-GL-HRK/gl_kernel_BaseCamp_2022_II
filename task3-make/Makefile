CC=gcc

flag_lib = "stat"

ifeq "${flag_lib}" "stat"
	mreq = game.o func.a
else
	mreq = game.o func.a
endif

target:
	@echo "To build use make all"

all: ${mreq}
	${CC} $^ -o game.out

main.o:game.c
	${CC} -c $^ -Wall

func.o:func.c
	${CC} -c $^ -Wall

func.a:func.o
	${CC} -Wall -c func.c
	ar rsc func.a func.o

clean:
	rm *.o  *.a *.out
