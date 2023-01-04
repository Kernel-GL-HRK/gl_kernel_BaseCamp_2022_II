# Compiler
CC = gcc

# Compiler options
CFLAGS =

# Options used in linking
LDFLAGS =

# Type of library to use (static or shared)
# Will be linked in regular way if not specified
LIBFLAG =

# Executable file name
PROGRAM = guess

sources = *.c *.h
objs = $(PROGRAM).o game.o
deps =


ifeq ($(LIBFLAG),static)
	objs = $(PROGRAM).o libgame.a
endif

ifeq ($(LIBFLAG),shared)
	CFLAGS += -fPIC
	LDFLAGS += $(CFLAGS) -lgame -L ./
	objs = $(PROGRAM).c
	deps = game.o libgame.so
endif

.PHONY: all check clean


all: $(PROGRAM)

debug: CFLAGS = -DDEBUG -O0 -g
debug: $(PROGRAM)

$(PROGRAM): $(objs) $(deps)
	$(CC) $(objs) $(LDFLAGS) -o $(PROGRAM)

%.o: %.c
	$(CC) $(CFLAGS) -c $^

lib%.a: %.o
	ar rcs $@ $^

lib%.so: %.o
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.0.1 $< -lc
	ln -s $@.1.0.1 $@.1
	ln -s $@.1 $@

check:
	checkpatch.pl --no-tree -f $(sources)

clean:
	rm -f $(PROGRAM) *.o *.a *.so*