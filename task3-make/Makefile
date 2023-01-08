CC = gcc

CPPFLAGS = -Wall -Werror

CFLAGS =

LDFLAGS =

LIBFLAG =

PROGRAM = main

objs = $(PROGRAM).o guess.o
deps =

CHECKFLAG =

ifeq ($(CHECKFLAG),cppcheck)
	CHECK_PROGRAM=cppcheck  --enable=all *.c *.h
endif

ifeq ($(CHECKFLAG),checkpatch)
	CHECK_PROGRAM=./checkpatch.pl -f --no-tree -f *.c *.h
endif

ifeq ($(LIBFLAG),static)
	objs = $(PROGRAM).o libguess.a
endif

ifeq ($(LIBFLAG),shared)
	CFLAGS += -fPIC
	LDFLAGS += $(CFLAGS) -lguess -L ./
	objs = $(PROGRAM).c
	deps = guess.o libguess.so
endif

.PHONY: all check clean


all: $(PROGRAM)

$(PROGRAM): $(objs) $(deps)
	$(CC) $(objs) $(LDFLAGS) -o $(PROGRAM)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $^

lib%.a: %.o
	ar rcs $@ $^

lib%.so: %.o
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.0.1 $< -lc
	ln -s $@.1.0.1 $@.1
	ln -s $@.1 $@

check:
	$(CHECK_PROGRAM)

clean:
	rm -f $(PROGRAM) *.o *.a *.so*
