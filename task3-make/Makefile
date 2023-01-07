CC 		= gcc
CLANG		= clang

# select gcc or clang
CSEL = gcc

# compiler selection
ifeq ($(CSEL), gcc)
	COMPILER = $(CC)
else ifeq ($(CSEL), clang)
	COMPILER = $(CLANG)
else
$(error Select compiler options)
endif

main: main.o random.o
	$(COMPILER) main.o random.o -o main

random.o: random.c random.h
	$(COMPILER) random.c -c -o random.o

main.o: main.c
	$(COMPILER) main.c -c -o main.o

