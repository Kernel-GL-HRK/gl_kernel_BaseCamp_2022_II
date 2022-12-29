SRC_DIR = ../task1-simple-program

SOURCES = $(shell find $(SRC_DIR) -name '*.c')
OBJECTS = $(SOURCES:.c=.o)

EXEC_PROG = $(SRC_DIR)/guessanumber

all: $(EXEC_PROG)

$(EXEC_PROG): $(OBJECTS)
	gcc $^ -o $@

$(SRC_DIR)/%.o:%.c
	gcc -c $<
