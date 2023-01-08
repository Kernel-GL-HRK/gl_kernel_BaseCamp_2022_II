CC = gcc

CPPFLAGS = -Wall -Werror

CFLAGS =

LDFLAGS =

LIBFLAG =

PROGRAM = main

objs = $(PROGRAM).o guess.o
deps =

ifeq ($(LIBFLAG),static)
	objs = $(PROGRAM).o libguess.a
endif

ifeq ($(LIBFLAG),shared)
	CFLAGS += -fPIC
	LDFLAGS += $(CFLAGS) -lguess -L ./
	objs = $(PROGRAM).c
	deps = guess.o libguess.so
endif

.PHONY: all clean


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

clean:
	rm -f $(PROGRAM) *.o *.a *.so*
