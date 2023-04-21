
CC=gcc


flag_check =
ifeq ($(flag_check),cppcheck)
	CHECK_PROGRAM=cppcheck  --enable=all *.c *.h
else
	CHECK_PROGRAM=../checkpatch.pl -f --no-tree -f *.c *.h
endif

flag_lib =
ifeq ($(flag_lib),static)
	mreq = guess_a_number.o get_computer_number.a
else
	mreq = guess_a_number.o get_computer_number.so
endif

flag_debug =
ifeq ($(flag_debug),debug)
	debug = -g
endif


#target:
#	echo "To build this project use make all command"
#	$(warning This is warning)


.PHONY:all

all: $(mreq)
	${CC} $^ -o guess_a_number

guess_a_number.o: guess_a_number.c
	${CC} -fPIC $(debug) -Wall -c $<

get_computer_number.a: get_computer_number.c
	${CC} -c $^
	ar rcs $@ get_computer_number.o

get_computer_number.so: get_computer_number.c
	${CC} -shared -Wl,-soname,$@.1 -o $@.1.0 $^ -lc
	ln -sf $@.1.0 $@.1
	ln -sf $@.1 $@


.PHONY: check
check:
	$(CHECK_PROGRAM)


.PHONY: clean
clean:
	rm -f guess_a_number *.o *.a *.so *.so.*
