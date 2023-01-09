TARGET 		= guessgame
CC 			= gcc
SOURCES 	=  main.c \
				utility.c
			
HEADERS 	= utility.h
CFLAGS      = -Wall
CHECKPATH   = /home/artem/Documents
CHECKFLAGS  = --no-tree
OBJ := ${SOURCES:c=o}

all: $(OBJ)

test:
	echo $(OBJ)

%.o : %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

guessgame: $(OBJ)
	$(CC) -o $@ $^

checkpatch:
	$(CHECKPATH)/checkpatch.pl $(CHECKFLAGS) $(SOURCES) 
