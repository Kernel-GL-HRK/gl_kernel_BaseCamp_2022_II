CC ?= gcc
CFLAGS ?= -Wall -Wextra
DEBAGFLAG ?= -g 

CHECK_STYLE ?= ./checkpatch.pl
ifeq ($(CHECK_STYLE),./checkpatch.pl) 
	CHECH_FLAGS := --no-tree -f
endif

# Variables to indicate whether we should build a static or a shared library.
BUILD_SHARED := 2
BUILD_STATIC := 1

# We want to have a rule to compile the executables.
# IF BUILD_SHARED != BUILD_STATIC
# We will make two versions of the rule, depending on whether we are linking
# against the shared or the static libraries.
# IF BUILD_SHARED == BUILD_STATIC
# We compile the file in a simple way.
ifneq ($(BUILD_SHARED),$(BUILD_STATIC))

ifeq ($(shell test $(BUILD_SHARED) -gt $(BUILD_STATIC); echo $$?),0)
guess: guess.o libmyshare.so
	$(CC) $(CFLAGS) -o $@ $< -L. -lmyshare
else
guess: guess.o libmystatic.a
	$(CC) $(CFLAGS) -o $@ $< -L. -lmystatic
endif

else
guess: guess.o functions-static.o
	$(CC) $(CFLAGS) -o $@ $^
endif
# End of the rule

guess.o: guess.c
	$(CC) $(CFLAGS) $(DEBAGFLAG) -c $< -o $@

functions-static.o: functions.c functions.h
	$(CC) $(CFLAGS) $(DEBAGFLAG) -c $< -o $@

functions-dynamic.o: functions.c functions.h
	$(CC) -fPIC $(CFLAGS) $(DEBAGFLAG) -c $< -o $@

libmystatic.a: functions-static.o
	ar rcs $@ $<

libmyshare.so: functions-dynamic.o
	$(CC) $(CFLAGS) -shared -Wl,-soname,libmyshare.so -o libmyshare.so $< -lc

.PHONY: check clean
check:
	$(CHECK_STYLE) $(CHECH_FLAGS) guess.c functions.c functions.h

clean:
	rm -f guess *.o *.so *.a
