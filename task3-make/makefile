CC=gcc
TEST=../checkpatch.pl --no-tree
SFILE=main.c

regular: main.o random.o
	${CC} -g $^ -o main.out

static: main.o librandom_st.a
	${CC} -g main.o -l random_st -L ./ -o main.out

dynamic_share: main.o librandom_sh.so
	${CC} -g main.o -L./ -Wl,-rpath=./ -lrandom_sh -o main.out

main.o: main.c
	${CC} -Wall -fPIC -c $^

librandom_st.a: random.o
	ar rsc $@ $<

librandom_sh.so: random.o
	${CC} -shared $< -o $@

random.o: random.c random.h
	${CC} -Wall -fPIC -c $<

run: regular
	./main.out
	rm -f *.out *.o *.a *.so

check:
	${TEST} ${SFILE}

.PHONY: run regular static dynamic_share check