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
EXTRA_CFLAGS	= 

# C linker flags
LDFLAGS		=
EXTRA_LDFLAGS	= 

# Library type and object files (when linking against library)
LIBTYPE 	=
LIB_OBJS	=
LIB		=

# For static library
ifeq ($(LIBTYPE), static)
    LIB		= libmyfunc.a
    LIB_OBJS	= $(addsuffix .o, $(basename $(filter-out main.c,$(SOURCES))))
    OBJS	= main.o
endif

# For shared library
ifeq ($(LIBTYPE), shared)
    EXTRA_LDFLAGS =-lmyfunc -L ./
    EXTRA_CFLAGS  = -fPIC
    LIB		= libmyfunc.so
    LIB_OBJS	= $(addsuffix .o, $(basename $(filter-out main.c,$(SOURCES))))
    OBJS	= main.o
endif

# Compile and link commands
COMPILE.c   	= $(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c
LINK.c 		= $(CC)

# Code style checker settings
CHECKER		= checkpatch.pl
ifeq ($(CHECKER), checkpatch.pl)
    CHECKFLAGS	= --no-tree -f
    CHECKER	= ./checkpatch.pl
else
    CHECKFLAGS<>= --enable=all
endif

# The command used to delete file.
RM 		= rm -f

.PHONY: all clean objs show target check help

# Default target
target:
	@echo 'Provide a target name to make.'
	@echo 'Type make help to get help.'

all: $(PROGRAM)

# Rules for generating object files (.o).
objs:$(OBJS) $(LIB_OBJS)

%.o:%.c
	$(COMPILE.c) $< -o $@

# Rules for generating static library (.a).
$(LIB):$(LIB_OBJS)
ifeq ($(LIBTYPE), static)
	ar rcs $@ $^
endif

# Rules for generating shared library (.so).
ifeq ($(LIBTYPE), shared)
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.0 $^ -lc
	ln -sf $@.1.0 $@.1
	ln -sf $@.1 $@
endif

# Rules for generating the executable.
$(PROGRAM):$(OBJS) $(LIB)
	$(LINK.c) $(OBJS) $(LIB) $(EXTRA_LDFLAGS) -o $@
	@echo Type ./$@ to execute the program.

# Rules for checking the source files (.c).
check:$(SOURCES)
	$(CHECKER) $(CHECKFLAGS) $^

clean:
	$(RM) $(OBJS) *.a *.so* $(PROGRAM)

# Target for makefile debug purpose
show:
	@echo 'PROGRAM     :' $(PROGRAM)
	@echo 'HEADERS     :' $(HEADERS)
	@echo 'SOURCES     :' $(SOURCES)
	@echo 'OBJS        :' $(OBJS)
	@echo 'COMPILE.c   :' $(COMPILE.c)
	@echo 'link.c      :' $(LINK.c)
	@echo 'LIB         :' $(LIB)
	@echo 'LIB_OBJS    :' $(LIB_OBJS)

# Show help.
help:
	@echo 
	@echo 'Usage: make [TARGET]'
	@echo 'TARGETS:'
	@echo '  all       (=make) compile and link.'
	@echo '  CC=gcc (default for gcc compiler) add CC=clang to the  make command to use clang compiler.'
	@echo '  LIBTYPE= (default for no library) add to the  make command LIBTYPE=static to link against static library (.a), or add'
	@echo '  LIBTYPE=shared to link against shared library (.so).'
	@echo '  check     check all source files code style (no compiling or linking).'
	@echo '  CHECKER=checkpatch.pl (default) add to the make command with target check CHECKER=cppcheck to use it for style checking.'
	@echo '  objs      compile only (no linking).'
	@echo '  clean     clean objects and the executable file.'
	@echo '  show      show variables (for debug use only).'
	@echo '  help      print this message.'
	@echo 

