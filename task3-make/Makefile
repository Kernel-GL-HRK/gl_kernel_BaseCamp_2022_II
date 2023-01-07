PROG		= main
CC 		= gcc
CLANG		= clang
CFLAGS		=
DEBUG		=
DEBUG_EXTRA	= -g

# select gcc or clang
CSEL = gcc

# link options for second file in:
# regular way 		= regular
# static library (.a)	= static
# dynamic library (.so) = dynamic
LOPT = regular

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

# linkd options for second file
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

.PHONY: all regular static dynamic cppcheck checkpatch debug release

all: $(prog)

cppcheck:
	cppcheck --enable=warning,style,performance,portability ./*.c ./*.h

checkpatch:
	$(CHECKPATCH_PATH) -f --no-tree ./*.c ./*.h
	
debug: DEBUG += $(DEBUG_EXTRA)
debug: $(prog)

release: $(prog)

regular: $(link_regular)
	$(COMPILER) $(CFLAGS) $(DEBUG) $^ -o $(PROG)

static: $(link_static)
	$(COMPILER) $(DEBUG) $< $(LIBNAME).a -o $(PROG)

dynamic: $(link_dynamic)
	$(COMPILER) $(DEBUG) $< $(LIBNAME).so -L ./ -o $(PROG)

%.o: %.c
	$(COMPILER) $(CFLAGS) $(DEBUG) -c $< -o $@

%.a: %.o
	ar rcs $(LIBNAME).a $<

%.so: %.o
	$(COMPILER) $(DEBUG) -shared -Wl,-soname,$(LIBNAME).so -o $(LIBNAME).so $< -lc

