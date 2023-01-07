CC = gcc # clang

guess_a_number: random.o guess_a_number.c
	$(CC) $^ -o $@ 

random.o: random.c
	$(CC) -c $<
