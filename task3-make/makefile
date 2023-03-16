Name=Vanga
CC=gcc
Link_like=
Flag=

Debug=false
Warning=false

ifeq (${Debug}, true)
	Flag += -g
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

clean:
	rm -f *.out *.o *.so *.a

help:
	$(info 	Variable "Name": Name of program, can be any str.)
	$(info 	Variable "CC": Compiler that will be using for compiling.)
	$(info 	Variable "Link_like": Responsible for the type of linking source code from not main file, can accept: )
	$(info 		- " "(nothing): regular compiling;)
	$(info 		- "_static": linking source code like static library;)
	$(info 		- "_dynamic": linking source code like dynamic library.)
	$(info 	Variable "Flag": Flags for compiling.)
	$(info 	)
	$(info 	Flag "Debug": Enables debug mode, can accept "true" or "false".)
	$(info 	Flag "Warning": Enables mode of print all warning during compilation, can accept "true" or "false".)
	$(info 	)
	$(info 	Option "target": Starts compilation according to the specified parameters.)
	$(info 	Option "check": Checks all project code using checkpatch.pl.)
	$(info 	Option "clean": Deletes all intermediate and resulting files.)
	$(info 	Option "help": Displays help information.)
	
