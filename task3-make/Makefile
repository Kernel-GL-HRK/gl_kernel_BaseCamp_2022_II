CC=gcc #-clang
CFLAGS= #-Wall -g
TARGET = mains
PREFIX = /usr/lib/
myPROGRAM=progRegular
flag_so= # -fPIC dynamic library 

CPPCHECK = cppcheck
CHECKFLAGS =--enable=all --inconclusive --check-config --template=gcc --suppress=missingIncludeSystem

flag_lib=0
ifeq ($(flag_lib),1)
    myPROGRAM=progStaticLib
endif
ifeq ($(flag_lib),2)
    myPROGRAM=progDynamicLib
	flag_so=-fPIC 
endif

target:
	@echo
	@echo "To buid this project use make all command"
	@echo "example: $$ make all CC=gcc # CC=clang Default (CC=gcc flag_lib=0)"
	@echo "example: $$ make all CFLAGS=-Wall # -Wall Enables all compiler's warning messages."
	@echo "example: $$ make all CFLAGS=\"-Wall -g\" # \"-Wall -g\" Enables both flags."
	@echo "example: $$ make all CFLAGS=-g # -g Generate debugging information"
	@echo "example: $$ make all flag_lib=0 # regular (default)"
	@echo "example: $$ make all flag_lib=1 # static library (.a)"
	@echo "example: $$ make all flag_lib=2 # dynamic library (.so)"
	@echo "example: $$ make install_so  # sudo cp ./libmylib.so /usr/lib/ for dynamic library (.so)"
	@echo "example: $$ make uninstall_so  # sudo rm /usr/lib/libmylib.so for dynamic library (.so)"
	@echo
	@echo "CPPCHECK"
	@echo "example: $$ make cppcheck # (*.c) CHECKFLAGS =--enable=all --inconclusive --check-config --template=gcc --suppress=missingIncludeSystem"
	@echo


.PHONY: all libs libs_so install_so uninstall_so cppcheck clean

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


install_so:
	sudo cp ./libmylib.so ${PREFIX}

uninstall_so:
	sudo rm ${PREFIX}libmylib.so

cppcheck: *.c
	$(CPPCHECK) $(CHECKFLAGS) $^ --xml > cppcheck.out.xml

clean:
	rm -f ${TARGET} *.o *.a *.so *.gch *.xml