# Compiler
CC = gcc

# Pre-processor options
CPPFLAGS = -Wall -Werror

# Compiler options
CFLAGS =

# Options used in linking
LDFLAGS =

# Type of library to use (static or shared)
# Will be linked in regular way if not specified
LIBFLAG =

# Executable file name
PROGRAM = guess

sources = $(wildcard *.[ch])
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

.PHONY: all check clean help


all: $(PROGRAM)

debug: CFLAGS = -DDEBUG -O0 -g
debug: $(PROGRAM)

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
	checkpatch.pl --no-tree -f $(sources)

clean:
	rm -f $(PROGRAM) *.o *.a *.so*

help:
	@echo 'Usage: make [TARGET] [FLAGS]'
	@echo 'TARGETS:'
	@echo '  all       (=make) compile and link'
	@echo '  debug     compile and link with debug information'
	@echo '  check     check source files with checkpatch.pl'
	@echo '  clean     clean objects and the executable file'
	@echo '  help      print this message'
	@echo
	@echo 'FLAGS:'
	@echo '  LIBFLAG=static'
	@echo '            link as static library'
	@echo '  LIBFLAG=shared'
	@echo '            link as shared library'
