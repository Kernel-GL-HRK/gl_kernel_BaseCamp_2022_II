CC	= gcc

PROGRAM	= guess
OBJS	= guess.o game.o

.PHONY: all clean


all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $^ -o $(PROGRAM)

*.o: *.c
	$(CC) -c $^

clean:
	rm $(PROGRAM) *.o