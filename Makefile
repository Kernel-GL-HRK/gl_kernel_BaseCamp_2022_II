# Default compiler
CC		=gcc
# Default executable name
PROGRAM 	= main.out
# All sources and headers in project directory (non recursive)
SOURCES 	= $(wildcard *.c)
HEADERS 	= $(wildcard *.h)
# Object files for all sources
OBJS 		= $(addsuffix .o, $(basename $(SOURCES)))
# C compiler flags
CFLAGS 		= -Wall -Wextra -g
CEXTFLAGS	= 

COMPILE.c   	= $(CC) $(CFLAGS) $(CEXTFLAGS) -c
LINK.c 		= $(CC)
# 
CHECKER		= checkpatch.pl
ifeq ($(CHECKER), checkpatch.pl)
    CHECKFLAGS	= --no-tree -f
    CHECKER	= ./checkpatch.pl
else
    CHECKFLAGS<>= --enable=all
endif

RM 		= rm -f

.PHONY: all clean objs show target check

target:
	@echo Provide a target name to make

all: $(PROGRAM)

objs:$(OBJS)

%.o:%.c
	$(COMPILE.c) $< -o $@


$(PROGRAM):$(OBJS)
	$(LINK.c)   $(OBJS) -o $@
	@echo Type ./$@ to execute the program.

check:$(SOURCES)
	$(CHECKER) $(CHECKFLAGS) $^

clean:
	$(RM) $(OBJS) $(PROGRAM)
# Target for makefile debug purpose
show:
	@echo 'PROGRAM     :' $(PROGRAM)
	@echo 'HEADERS     :' $(HEADERS)
	@echo 'SOURCES     :' $(SOURCES)
	@echo 'OBJS        :' $(OBJS)
	@echo 'COMPILE.c   :' $(COMPILE.c)
	@echo 'link.c      :' $(LINK.c)


