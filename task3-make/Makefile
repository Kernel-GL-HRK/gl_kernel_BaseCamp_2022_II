PROG		= main
CC 		= gcc
CLANG		= clang
CFLAGS		=
CFLAGS_EXTRA	= -Werror -Wall -Wextra -Wsign-conversion
DEBUG		=
DEBUG_EXTRA	= -g

# select gcc or clang
CSEL = gcc

# link options for second file in:
# regular way 		= regular
# static library (.a)	= static
# dynamic library (.so) = dynamic
LOPT = regular

# select debug information debug or release
DSEL = debug

# select warning during compilation warning or none
WSEL = warning

# library name for static and dynamic link
LIBNAME 	= libmylib

DYNAMIC_EXTRA	= -fPIC

link_regular 	= main.o random.o
link_static  	= main.o random.a
link_dynamic 	= main.c random.o random.so

CHECKPATCH_PATH = ./checkpatch.pl

# compiler selection
ifeq ($(CSEL), gcc)
	COMPILER = $(CC)
else ifeq ($(CSEL), clang)
	COMPILER = $(CLANG)
else
$(error Select compiler options)
endif

# linked options for second file
ifeq ($(LOPT), regular)
	link_option = $(link_regular)
	prog = regular
else ifeq ($(LOPT), static)
	link_option = $(link_static)
	prog = static
else ifeq ($(LOPT), dynamic)
	CFLAGS += $(DYNAMIC_EXTRA)
	link_option = $(link_dynamic)
	prog = dynamic
else
$(error Select link options for second file)
endif

# debug options
ifeq ($(DSEL), debug)
	DEBUG += $(DEBUG_EXTRA)
else ifeq ($(DSEL), release)
	DEBUG = $()
else
$(error Select debug options)
endif

# warning options
ifeq ($(WSEL), warning)
	CFLAGS += $(CFLAGS_EXTRA)
endif

.PHONY: all regular static dynamic cppcheck checkpatch

all: $(prog)

cppcheck:
	cppcheck --enable=warning,style,performance,portability ./*.c ./*.h

checkpatch:
	$(CHECKPATCH_PATH) -f --no-tree ./*.c ./*.h

regular: $(link_regular)
	$(COMPILER) $(CFLAGS) $(DEBUG) $^ -o $(PROG)

static: $(link_static)
	$(COMPILER) $(CFLAGS) $(DEBUG) $< $(LIBNAME).a -o $(PROG)

dynamic: $(link_dynamic)
	$(COMPILER) $(CFLAGS) $(DEBUG) $< $(LIBNAME).so -L ./ -o $(PROG)

%.o: %.c
	$(COMPILER) $(CFLAGS) $(DEBUG) -c $< -o $@

%.a: %.o
	ar rcs $(LIBNAME).a $<

%.so: %.o
	$(COMPILER) $(CFLAGS) $(DEBUG) -shared -Wl,-soname,$(LIBNAME).so -o $(LIBNAME).so $< -lc

