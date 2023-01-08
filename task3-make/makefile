Name=Vanga
CC=gcc
Link_like=

target: ${Name}${Link_like}
	
${Name}: main.o get_rand.o
	${CC} $^ -o $@.out

${Name}_static: main.o get_rand.a
	${CC} $^ -o $@.out

${Name}_dynamic: main.o lib_get_rand.so
	${CC} $^ -o $@.out

get_rand.a: get_rand.o 
	ar rcs $@ $<

%.so: %.o
	${CC} -shared -Wl,-soname,./$@ -o $@ $^ -lc

lib_%.o: ../task1-simple-program/%.c
	${CC} -fPIC -c $< -o $@

%.o: ../task1-simple-program/%.c

	${CC} -c $<


