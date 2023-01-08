CC = gcc
CFLAGS = -Wall -Werror -Wextra 
DEBUG_EXTRAFLAGS = -g

PROG_NAME = guess_a_number
MAIN = guess_a_number.c
SRCMODULES = random.c
OBJMODULES = $(SRCMODULES:.c=.o)

SLIB = librandom.a
DLIB = librandom.so

CHECK_PROG = ./checkpatch.pl
CHECK_FLAGS = --no-tree -f

ifeq ($(CHECK_PROG), cppcheck)
	CHECK_FLAGS = --enable=all --inconclusive --check-config --template=gcc --suppress=missingIncludeSystem
endif

LIB =

ifeq ($(LIB), static)
	PROG_BUILD = static
else ifeq ($(LIB), dynamic)
	PROG_BUILD = dynamic
else
	PROG_BUILD = regular
endif

#for MacOs
DLIB_BUILD = $(CC) $< -lrandom -L ./ -o $(PROG_NAME)
OS =
ifeq ($(OS), linux)
       	DLIB_BUILD = $(CC) -shared -Wl,-soname,$(DLIB) -o $(DLIB) $(OBJMODULES) -lc
endif

help: 
	@echo
	@echo "To build project use <make all> (gcc default and regular way)\n"
	@echo "To build project with clang use <make all CC=clang> (gcc default)\n"
	@echo "To include debug information use <make debug>\n"
	@echo "To check all project code use: <make check> (checkpatch.pl default)"
	@echo "                               <make check CHECK_PROG=checkpatch.pl>"
	@echo "                               <make check CHECK_PROG=cppcheck>\n"
	@echo "To link second files with: static library (.a) use <make all LIB=static>"
	@echo "                           dynamic library(.so) use <make all LIB=dynamic>"
	@echo "                           regular way use <make all> or <make all LIB=regular\n"
	@echo "To build your project with dynamic library on Linux use flag <OS=linux> (MacOs default)\n"

all: $(PROG_BUILD)

./PHONY: debug check run clean regular static all help

regular: $(OBJMODULES) $(MAIN)
	$(CC) $(CFLAGS) $^ -o $(PROG_NAME)


static: $(SLIB) $(MAIN)
	$(CC) $(CFLAGS) $^ -o $(PROG_NAME)

$(SLIB): $(OBJMODULES)
	ar rcs $@ $<


dynamic: $(MAIN) $(DLIB)
	$(DLIB_BUILD)

$(DLIB): $(SRCMODULES)
	$(CC) -fPIC -c $<
	$(CC) -dynamiclib -o $@ $(OBJMODULES)
	

$(OBJMODULES): $(SRCMODULES)
	$(CC) $(CFLAGS) -c $<


debug: CFLAGS += $(DEBUG_EXTRAFLAGS)

debug: $(PROG_NAME)

check:
	$(CHECK_PROG) $(CHECK_FLAGS) ./*.[ch]

run:
	./$(PROG_NAME)

clean:
	rm -rf *.a *.o *.so $(PROG_NAME) *.dSYM

