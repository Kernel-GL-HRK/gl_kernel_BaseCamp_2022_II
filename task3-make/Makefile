CC = gcc # clang
CFLAGS = -Wall -Werror -Wextra 

PROG=guess_a_number

./PHONY: clean

all: $(PROG)

$(PROG): random.o guess_a_number.c
	$(CC) $(CFLAGS) $^ -o $@ 

random.o: random.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(PROG)
