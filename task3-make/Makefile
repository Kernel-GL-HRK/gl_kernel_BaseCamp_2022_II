CC=gcc
CFALGS=-g -Wall
#required "export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH" to work with .so n the local directory 

#dynlib has to be mentioned as a dependencies the in target all
#libs - as an option at linking
LDFLAGS = 
LDLIBS = 
flag_lib = 

#ccheck = 
#ccheck_flags = 
ccheck = cppcheck
ccheck_flags = --enable=all --output-file="check.log"
ifeq (${flag_lib}, dyn)
	mreq = game.o
	LDFLAGS = -L./ -lfunc
	LDLIBS = libfunc.so
else ifeq (${flag_lib}, stat)
	mreq = game.o func.a
else
	mreq = game.o func.o
endif

ccheck:
	${ccheck} ${ccheck_flags} *.c *.h
target:
	@echo "To build use make all"

all:${mreq} ${LDLIBS}
	${CC} ${CFALGS} ${mreq} -o game.out ${LDFLAGS} 

game.o:game.c
	${CC} ${CFALGS} -c game.c

func.o:func.c
	${CC} ${CFALGS} -c $^ 

func.a:func.o
	${CC} ${CFALGS} -c func.c
	ar rsc func.a func.o

#create  dynlink lib
libfunc.so:func.o
	@echo "Making dynlink lib"
	${CC} ${CFALGS} -fPIC -shared -Wl,-soname,libfunc.so.1 -o libfunc.so.1.0   func.o
	ln -sf libfunc.so.1.0 libfunc.so.1
	ln -sf libfunc.so.1.0 libfunc.so
clean:
	rm *.o  *.a *.out *.so*
