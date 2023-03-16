CC =

CHECKER = ../scripts/checkpatch.pl
CHECKER_FLAGS = --no-tree -f

PROGRAM = main

DEBUG_FLAGS = -DDEBUG -g
WARN_FLAGS = -Wall -Wextra -Wpedantic

EXTRA_LDFLAGS =
EXTRA_CFLAGS  = -fdata-sections -ffunction-sections
CPPFLAGS      =
CFLAGS        =

SRCDIR  = ../task1-simple-program
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/get_random_number.c
HEADERS = $(SRCDIR)/get_random_number.h
OBJS    = $(addsuffix .o, $(basename $(SOURCES)))

LIBTYPE =

ifeq ($(LIBTYPE),static)
	DEPS          = libfunc.a
endif

ifeq ($(LIBTYPE),shared)
	EXTRA_LDFLAGS = -lfunc -L ./
	CFLAGS        = -fPIC
	DEPS          = libfunc.so
endif

COMPILE.c   = $(CC)  $(EXTRA_CFLAGS) $(CFLAGS)   $(CPPFLAGS) -c
LINK.c      = $(CC)  $(EXTRA_CFLAGS) $(CPPFLAGS) $(LDFLAGS)

.PHONY: build debug warn check clean help

build:$(PROGRAM)

debug:CPPFLAGS += $(DEBUG_FLAGS)
debug:$(PROGRAM)

warn:CPPFLAGS += $(WARN_FLAGS)
warn:$(PROGRAM)

$(PROGRAM):$(OBJS) $(DEPS)
	$(LINK.c) $(OBJS) $(EXTRA_LDFLAGS) -o $@
	@echo Type ./$@ to execute the program.

objs:$(OBJS)

%.o:%.c
	$(COMPILE.c) $< -o $@

lib%.so:%.o
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.0 $^ -lc
	ln -sf $@.1.0 $@.1
	ln -sf $@.1 $@

lib%.a:%.o
	ar rcs $@ $^

check:
	$(foreach d,$(SOURCES),$(CHECKER) $(CHECKER_FLAGS) $(d))

clean:
	rm -f $(OBJS) $(PROGRAM) *.a *.so $(PROGRAM).out

help:
	@echo 'Usage: make [TARGET]'
	@echo 'TARGETS:'
	@echo '  build     (=make) compile and link.'
	@echo '  debug     build with debug info.'
	@echo '  warn      build with all warnings.'
	@echo '  objs      compile only (no linking).'
	@echo '  clean     clean objects and the executable file.'
	@echo '  help      print this message.'
