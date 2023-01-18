.PHONY : all check clean

all: guesanumber

# CC = gcc or clang
CC = gcc

# warn = -Wall //compile with -Wall
warn =

# debug = -g //compile with -g
debug =

#==============================================================================
# check-target
#==============================================================================

# check_util = checkpatch or cppcheck
check_util = checkpatch

# Form command for check-target
ifeq (${check_util},checkpatch)
  check_cmd = ./checkpatch.pl --no-tree -f $$( find . -name "*.c" -type f)
else ifeq (${check_util},cppcheck)
  check_cmd = cppcheck -q --enable=all $$( find . -name "*.c" -type f)
else
  check_cmd = @echo Bad argument check_util = $${check_util}
endif

check:
	$(check_cmd)
#	for i in $$( find . -name "*.c" -type f); do ./checkpatch.pl --no-tree -f $${i}; echo "\n"; done
#	$(foreach c_file,$(wildcard *.c),echo "\n------\n";./checkpatch.pl --no-tree -f $(c_file);)
#	./checkpatch.pl --no-tree -f $(wildcard *.c)
#	./checkpatch.pl --no-tree -f $$( find . -name "*.c" -type f)
#	cppcheck -q --enable=all $$( find . -name "*.c" -type f)

#==============================================================================
# clean-target
#==============================================================================

clean_pattern = -name guesanumber -o -name '*.o' -o -name '*.a' -o -name '*.so.*'  -o -name '*.so'

clean:
#	rm -f guesanumber *.o *.a *.so.* *.so
	find . \( $(clean_pattern) \) -type f,l -print | xargs /bin/rm -f

#==============================================================================
# guesanumber-target
#==============================================================================

# flag_lib = stat //compile with static library
# flag_lib = shar //compile with shared library
# flag_lib = <any other value> //compile as regular file
flag_lib = none

# For guesanumber-target form requirements and command parameters
ifeq (${flag_lib},stat)
  req = main.o librand10.a
  gueslink = main.o -lrand10 -L ./
else ifeq (${flag_lib},shar)
  req = main.o librand10.so
  gueslink = main.o -lrand10 -L ./
else
  req = main.o rand10.o
  gueslink = $(req)
endif

guesanumber: $(req)
	${CC} $(gueslink) -o $@

main.o: main.c rand10.h
	${CC} ${debug} ${warn} -c $<

rand10.o: rand10.c
	${CC} ${debug} ${warn} -c $<

librand10.a: rand10.c
	${CC} ${debug} ${warn} -c $<
	ar rcs $@ rand10.o

librand10.so: rand10.c
	${CC} ${debug} ${warn} -fPIC -c $<
	${CC} -shared -Wl,-soname,librand10.so.1 -o librand10.so.1.0 rand10.o -lc
	ln -s librand10.so.1.0 librand10.so.1
	ln -s librand10.so.1 librand10.so
