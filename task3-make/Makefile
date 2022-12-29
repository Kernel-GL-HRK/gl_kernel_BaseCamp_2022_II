SRC_DIR = ../task1-simple-program

SOURCES = $(shell find $(SRC_DIR) -name '*.c')
OBJECTS = $(SOURCES:.c=.o)

EXEC_PROG = $(SRC_DIR)/guessanumber

CC=gcc
CFLAGS=-Wall -g

all: $(EXEC_PROG)

$(EXEC_PROG): $(OBJECTS)
	$(CC) $^ -o $@

$(SRC_DIR)/%.o:%.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean

clean:
	rm $(SRC_DIR)/*.o
