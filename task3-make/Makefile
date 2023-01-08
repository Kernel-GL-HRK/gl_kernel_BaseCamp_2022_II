#------------------------------------------
# makefile for guessanumber
#-----------------------------------------

CC ?= gcc
CFLAGS =
LDARGS =
PROGRAM = guessanumber
CURRENT_DIR = $(shell pwd)
DEBUG =
LINK =
CHECK =


#include debug information option check (DEBUG=true, by default false)
ifeq ($(DEBUG),true)
	CFLAGS +=  -g
endif

#Compile with library (LINK=static for making of static library, LINK=dynamic for dynamic library, by default compile regular way)
ifeq ($(LINK),static)
	link_obj_1=task1.o
	link_obj_2=librandom_func.a
else ifeq ($(LINK),dynamic)
	LDARGS += -L./
	link_obj_1=task1.o
	link_obj_2=librandom_func.so
else
	link_obj_1=task1.o
	link_obj_2=random_func.o
endif

#Complie with warnings flag (WARNINGS=all, WARNINGS=Extra, Default: no warnings)
ifeq ($(WARNINGS),all)
	CFLAGS = -Wall
else ifeq ($(WARNINGS),extra)
	CFLAGS = -Wall -Wextra
endif


#Check source code (CHECK=cppcheck to use cppcheck for game code checking, CHECK=checkpatch to use cppcheck for game code checking, default is checkpatch)

ifneq ($(CHECK),)
ifeq ($(CHECK), cppcheck)
	CHECK_PROG = cppcheck
	CHECK_FLAGS = --enable=all --inconclusive --check-config --template=gcc --suppress=missingIncludeSystem
else
	CHECK_PROG = ./checkpatch.pl
	CHECK_FLAGS = --no-tree -f
endif
endif

all: check $(PROGRAM)


$(PROGRAM): $(link_obj_1) $(link_obj_2)
	${CC} $(CFLAGS) $(link_obj_1) $(LDARGS) $(link_obj_2) -o $(PROGRAM)

task1.o:
	${CC} $(CFLAGS) -c $(CURRENT_DIR)/../task1-simple-program/task1.c

random_func.o:
	${CC} $(CFLAGS) -c $(CURRENT_DIR)/../task1-simple-program/random_func.c

librandom_func.a:random_func.o
	ar rcs $@ random_func.o

librandom_func.so:
	${CC} $(CFLAGS) -fPIC -c $(CURRENT_DIR)/../task1-simple-program/random_func.c
	${CC} $(CFLAGS) -shared -o $@ random_func.o

help:
	@echo "Flags and variables:"
	@echo " 'CC'	values: 'clang' to use clang compiler"
	@echo "		By Default files will be compiled by gcc"
	@echo " 'LINK'	values: 'static' to create the static lib"
	@echo "		'dynamic' to create the dynamic lib"
	@echo "		By Default files will be linked the regular way"
	@echo " 'DEBUG' values: 'true' to add debug information"
	@echo "		By Default is false"
	@echo " 'WARNINGS' values: 'all' to display all warnings"
	@echo "		'extra' display warnings not showen by all option"
	@echo "		By Default is disabled"
	@echo " 'CHECK' values: 'cppcheck' to use cppcheck source code check, other values to use checkpatch.pl."
	@echo "		Check will not run if value is not defined"

target:all

check:

ifneq ($(CHECK),)
	$(CHECK_PROG) $(CHECK_FLAGS) $(CURRENT_DIR)/../task1-simple-program/*.[ch]
endif


.PHONY: clean

clean:
	rm -f guessanumber *.o *.a *.so
