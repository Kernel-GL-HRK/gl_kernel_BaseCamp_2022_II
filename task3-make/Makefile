SRC_DIR = ../task1-simple-program

MAIN_SRC = main.c
MAIN_OBJ = $(MAIN_SRC:.c=.o)
OBJ = $(addprefix $(SRC_DIR)/, $(MAIN_OBJ))
SOURCES = $(shell find $(SRC_DIR) -name '*.c' ! -name '$(MAIN_SRC)')
OBJECTS = $(SOURCES:.c=.o)

EXEC_PROG = $(SRC_DIR)/guessanumber

CC=gcc
CFLAGS=-Wall -g
DEP_LIB=
LIBS=
LIB_OBJ=
LDFLAGS=$(LIBS) -L$(SRC_DIR)
LIB_TYPE=no

ifeq ($(LIB_TYPE), static)
DEP_LIB=$(SRC_DIR)/libguess.a
LIBS=-lguess
LIB_OBJ:=$(OBJECTS)
OBJECTS=
endif

ifeq ($(LIB_TYPE), shared)
DEP_LIB=$(SRC_DIR)/libguess.so
LIBS=-lguess
LIB_OBJ:=$(OBJECTS)
OBJECTS=
endif

all: $(EXEC_PROG)

$(EXEC_PROG): $(OBJ) $(OBJECTS) $(DEP_LIB)
	$(CC) $(OBJ) $(OBJECTS) $(LDFLAGS) -o $@

$(SRC_DIR)/%.o:%.c
	$(CC) $(CFLAGS) -c $<

$(SRC_DIR)/libguess.a: $(LIB_OBJ)
	ar rcs $@ $^

.PHONY: clean

clean:
	rm $(SRC_DIR)/*.o
