CC = gcc

PROGRAM = main

EXTRA_LDFLAGS =
EXTRA_CFLAGS  = -fdata-sections -ffunction-sections
CPPFLAGS      = -Wall -Wextra -DDEBUG -g
CFLAGS        =

SRCDIR  = ../task1-simple-program
SOURCES = $(foreach d,$(SRCDIR),$(wildcard $(addprefix $(d)/*,.c)))
HEADERS = $(foreach d,$(SRCDIR),$(wildcard $(addprefix $(d)/*,.h)))
SRC_CXX = $(filter-out %.c,$(SOURCES))
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

.PHONY: all objs clean help

all:$(PROGRAM)

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
