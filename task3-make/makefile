Name=Vanga
CC=gcc
Link_like=
Flag=

Debag=false
Warning=false

ifeq (${Debag}, true)
	Flag=-g
endif

ifeq (${Warning}, true)
	Flag += -Wall
endif

target: ${Name}${Link_like}
	
${Name}: main.o get_rand.o
	${CC} ${Flag} $^ -o $@.out

${Name}_static: main.o get_rand.a
	${CC} ${Flag} $^ -o $@.out

${Name}_dynamic: main.o lib_get_rand.so
	${CC} ${Flag} $^ -o $@.out

get_rand.a: get_rand.o 
	ar rcs $@ $<

%.so: %.o
	${CC} ${Flag} -shared -Wl,-soname,./$@ -o $@ $^ -lc

lib_%.o: ../task1-simple-program/%.c
	${CC} ${Flag} -fPIC -c $< -o $@

%.o: ../task1-simple-program/%.c

	${CC} ${Flag} -c $<

check: 
	../checkpatch.pl --no-tree ../task1-simple-program/*.c

