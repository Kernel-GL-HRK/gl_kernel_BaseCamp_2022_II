#------------------------------------------
# makefile for guessanumber
#-----------------------------------------

CC ?= gcc
CFLAGS ?= -Wall
PROGRAM = guessanumber
FILES.o = task1.o random_func.o
FILES.c = task1.c random_func.c
FILES.h = task1.h
CURRENT_DIR = $(shell pwd)

$(warning $(CURRENT_DIR))

all: $(PROGRAM)

$(PROGRAM): $(FILES.o)
	${CC} -Wall  $(FILES.o) -o $(PROGRAM)

task1.o:
	${CC} -Wall -c $(CURRENT_DIR)/../task1-simple-program/task1.c

random_func.o:
	${CC} -Wall -c $(CURRENT_DIR) /../task1-simple-program/random_func.c

.PHONY: clean

clean:
	rm -f guessanumber *.o
