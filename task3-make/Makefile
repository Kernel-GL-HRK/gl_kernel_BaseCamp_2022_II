SRC_DIR = ../task1-simple-program

MAIN_SRC = main.c
MAIN_OBJ = $(MAIN_SRC:.c=.o)
OBJ = $(addprefix $(SRC_DIR)/, $(MAIN_OBJ))
SOURCES = $(shell find $(SRC_DIR) -name '*.c' ! -name '$(MAIN_SRC)')
OBJECTS = $(SOURCES:.c=.o)

EXEC_PROG = $(SRC_DIR)/guessanumber

CC=gcc
CFLAGS=-Wall -g
LIB=

all: $(EXEC_PROG)

$(EXEC_PROG): $(OBJ) $(OBJECTS) $(LIB)
	$(CC) $^ -o $@

$(SRC_DIR)/%.o:%.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean

clean:
	rm $(SRC_DIR)/*.o
