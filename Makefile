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

clean_pattern = -name guesanumber -o -name '*.o'

clean:
#	rm -f guesanumber *.o *
	find . \( $(clean_pattern) \) -type f,l -print | xargs /bin/rm -f

#==============================================================================
# guesanumber-target
#==============================================================================

guesanumber: main.o
	${CC} $^ -o $@

main.o: main.c
	${CC} ${debug} ${warn} -c $<
