Name=Vanga
CC=gcc

${Name}: main.o get_rand.o
	${CC} $^ -o $@.out

%.o: ../task1-simple-program/%.c
	${CC} -c $<


