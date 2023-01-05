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

