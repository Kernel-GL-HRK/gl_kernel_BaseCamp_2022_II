
LINKFLAG = 
CC = gcc
CFLAG = -Wall
LDFLAG = 
LIBFLAG =
 
ifeq "${LINKFLAG}" "stat"
	MREQ = game.o libfunc.a
	#LDFLAG = -L. -lfunc
	LDFLAG = -static
else ifeq "${LINKFLAG}" "dyn"
	MREQ = game.o
	LDFLAG = -L. -lfunc
	LIBFLAG = libfunc.so
else
	MREQ = game.o func.o
endif


all:${MREQ} ${LIBFLAG}
	${CC} ${CFLAG} ${MREQ} -o game.out ${LDFLAG}
game.o:game.c
	${CC} ${CFLAG} -c  $^
func.o:func.c
	${CC} ${CFLAG} -c  $^
libfunc.a:func.o
	@echo "Creating static library $@"
	${CC} -Wall -c func.c
	ar rsc libfunc.a func.o
libfunc.so:
	@echo "Creating dynamic link library $@"
	gcc -fPIC -c func.c
	gcc -shared -Wl,-soname,libfunc.so.1 -o libfunc.so.1.0   func.o -lc
	ln -sf libfunc.so.1.0 libfunc.so.1
	ln -sf libfunc.so.1.0 libfunc.so

check:
	@./checkpatch.pl -f --no-tree *.c *.h > checkpatch.log
	@cppcheck  --enable=all --output-file=check.log *.c *.h
	@echo "Writing results to cppcheck.log"

.PHONY:clean
clean:
	rm *.o *.a *.out *.so*
