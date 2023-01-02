PROGRAM	= guess
OBJS	= guess.o game.o

.PHONY: all clean


all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	gcc $^ -o $(PROGRAM)

*.o: *.c
	gcc -c $^

clean:
	rm $(PROGRAM) *.o