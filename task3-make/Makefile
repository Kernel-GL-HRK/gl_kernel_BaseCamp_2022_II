#------------------------------------------
# makefile for guessanumber
#-----------------------------------------

CC ?= gcc
CFLAGS ?= -Wall
LDARGS =
PROGRAM = guessanumber
CURRENT_DIR = $(shell pwd)
DEBUG =
LINK=

ifeq ($(DEBUG),true)
	CFLAGS +=  -g
endif

ifeq ($(LINK),static)
	link_obj_1=task1.o
	link_obj_2=librandom_func.a
else ifeq ($(LINK),dynamic )
	LDARGS += -L./
	link_obj_1=task1.o
	link_obj_2=librandom_func.so
else
	link_obj_1=task1.o
	link_obj_2=random_func.o
endif


all: $(PROGRAM)

$(PROGRAM): $(link_obj_1) $(link_obj_2)
	${CC} $(CFLAGS) $(link_obj_1) $(LDARGS) $(link_obj_2) -o $(PROGRAM)

task1.o:
	${CC} $(CFLAGS) -c $(CURRENT_DIR)/../task1-simple-program/task1.c

random_func.o:
	${CC} $(CFLAGS) -c $(CURRENT_DIR)/../task1-simple-program/random_func.c

librandom_func.a:random_func.o
	ar rcs $@ random_func.o

librandom_func.so:
	${CC} -fPIC -c $(CURRENT_DIR)/../task1-simple-program/random_func.c
	${CC} -shared -o $@ random_func.o

help:
	@echo "Options:"
	@echo " 'LINK'	values: 'static' to create the static lib"
	@echo "		'dynamic' to create the dynamic lib"
	@echo "		By Default files will linked regular way"
	@echo " 'DEBUG' values: 'true' to add debug information"
	@echo "		By Default is false"

.PHONY: clean

clean:
	rm -f guessanumber *.o *.a *.so
