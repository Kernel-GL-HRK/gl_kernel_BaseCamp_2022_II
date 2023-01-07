CC = gcc # clang
CFLAGS = -Wall -Werror -Wextra 
DEBUG_EXTRAFLAGS = -g

PROG=guess_a_game

CHECK_PROG = ./checkpatch.pl
CHECK_FLAGS = --no-tree -f

ifeq ($(CHECK_PROG), cppcheck)
	CHECK_FLAGS = --enable=all --inconclusive --check-config --template=gcc --suppress=missingIncludeSystem
endif

PROG=guess_a_number

./PHONY: clean all debug check

all: $(PROG)

$(PROG): random.o guess_a_number.c
	$(CC) $(CFLAGS) $^ -o $@ 

random.o: random.c
	$(CC) $(CFLAGS) -c $<

debug: CFLAGS += $(DEBUG_EXTRAFLAGS)
debug: $(PROG)

check:
	$(CHECK_PROG) $(CHECK_FLAGS) ./*.[ch]

clean:
	rm -rf *.o $(PROG) *.dSYM

