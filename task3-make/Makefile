BUILD_SHARED = 1
BUILD_STATIC = 1

# Finally, we want to have a rule to compile the executables.
# IF BUILD_SHARED != BUILD_STATIC
# We will make two versions of the rule, depending on whether we are linking
# against the shared or the static libraries.
# IF BUILD_SHARED == BUILD_STATIC
# We compile the file in a simple way.
ifneq ($(BUILD_SHARED),$(BUILD_STATIC))

ifeq ($(shell test $(BUILD_SHARED) -gt $(BUILD_STATIC); echo $$?),0)
guess: guess.o libmyshare.so
	gcc -g -o guess guess.o -L. -lmyshare
else
guess: guess.o libmystatic.a
	gcc -g -o guess guess.o -L. -lmystatic
endif

else
guess: guess.o functions-static.o
	gcc -g -o guess guess.o functions-static.o
endif

guess.o: guess.c
	gcc -Wall -g -c -o guess.c guess.o

functions-static.o: functions.c functions.h
	gcc -Wall -g -c -o functions-static.o functions.c

functions-dynamic.o: functions.c functions.h
	gcc -fPIC -Wall -g -c -o functions-dynamic.o functions.c

libmystatic.a: functions-static.o
	ar rcs libmystatic.a functions-static.o

libmyshare.so: functions-dynamic.o
	gcc -g -shared -Wl,-soname,libmyshare.so -o libmyshare.so functions-dynamic.o -lc

check:
	./../checkpatch.pl --no-tree -f guess.c functions.c functions.h

clean:
	rm -f guess *.o *.so *.a
