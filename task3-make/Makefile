CC = gcc
CFLAG = -Wall
mreq = game.o func.o

all: ${mreq}
	${CC} ${CFLAG} $^ -o game.out
game.o:game.c
	${CC} ${CFLAG} -c  $^
func.o:func.c
	${CC} ${CFLAG} -c  $^
