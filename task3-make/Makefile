CC = gcc # clang
CFLAGS = -Wall -Werror -Wextra 
DEBUG_EXTRAFLAGS = -g

PROG=guess_a_number

./PHONY: clean all debug

all: $(PROG)

$(PROG): random.o guess_a_number.c
	$(CC) $(CFLAGS) $^ -o $@ 

random.o: random.c
	$(CC) $(CFLAGS) -c $<

debug: CFLAGS += $(DEBUG_EXTRAFLAGS)

debug: $(PROG)

clean:
	rm -rf *.o $(PROG) *.dSYM
