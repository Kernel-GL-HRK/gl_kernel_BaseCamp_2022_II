TARGET 		= guessgame
CC 			= gcc
SOURCES 	=  main.c \
				utility.c
			
HEADERS 	= utility.h
CFLAGS      :=
CHECKPATH   = /home/artem/Documents
CHECKFLAGS  = --no-tree
OBJ := ${SOURCES:c=o}

ifdef clang
	CC=clang
endif

ifdef debug
	CFLAGS := $(CFLAGS) -g
endif

ifdef Wall
	CFLAGS := $(CFLAGS) -Wall
endif

.PHONY: all test guessgame checkpatch clean

all: $(OBJ) guessgame

test:
	echo $(OBJ)

%.o : %.c
	$(CC) -o $@ -c $(CFLAGS) $<

guessgame: $(OBJ)
	$(CC) -o $@ $^

libguessgame.so: utility.o
	$(CC) -o $@ $^ -fPIC -shared

libguessgame.a: utility.o
	ar rcs $@ $<

checkpatch:
	$(CHECKPATH)/checkpatch.pl $(CHECKFLAGS) $(SOURCES) 

clean:
	rm -f *.o *.a *.so $(TARGET)
