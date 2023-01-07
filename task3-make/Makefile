CC = gcc # clang

PROG=guess_a_number

./PHONY: clean

all: $(PROG)

$(PROG): random.o guess_a_number.c
	$(CC) $^ -o $@ 

random.o: random.c
	$(CC) -c $<

clean:
	rm -f *.o $(PROG)
