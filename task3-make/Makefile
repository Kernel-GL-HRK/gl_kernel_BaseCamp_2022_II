
LINKFLAG =
CC = gcc
CFLAG = -Wall
LDFLAG = 

ifeq "${LINKFLAG}" "stat"
	MREQ = game.o libfunc.a
	#LDFLAG = -L. -lfunc
	LDFLAG = -static
else
	MREQ = game.o func.o
endif

all: ${MREQ}
	${CC} ${CFLAG} $^ -o game.out ${LDFLAG}
game.o:game.c
	${CC} ${CFLAG} -c  $^
func.o:func.c
	${CC} ${CFLAG} -c  $^
libfunc.a:func.o
	@echo "Creating static library $@"
	${CC} -Wall -c func.c
	ar rsc libfunc.a func.o

.PHONY:clean
clean:
	rm *.o *.a *.out
