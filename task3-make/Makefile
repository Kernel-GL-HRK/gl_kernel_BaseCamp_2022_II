SRC_DIR = ../task1-simple-program

MAIN_SRC = main.c
MAIN_OBJ = $(MAIN_SRC:.c=.o)
OBJ = $(addprefix $(SRC_DIR)/, $(MAIN_OBJ))
SOURCES = $(shell find $(SRC_DIR) -name '*.c' ! -name '$(MAIN_SRC)')
OBJECTS = $(SOURCES:.c=.o)
ALL_SOURCES = $(shell find $(SRC_DIR) -name '*.c')

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
CFLAGS+=-fPIC
endif

all: $(EXEC_PROG)

$(EXEC_PROG): $(OBJ) $(OBJECTS) $(DEP_LIB)
	$(CC) $(OBJ) $(OBJECTS) $(LDFLAGS) -o $@

$(SRC_DIR)/%.o:%.c
	$(CC) $(CFLAGS) -c $<

$(SRC_DIR)/libguess.a: $(LIB_OBJ)
	ar rcs $@ $^

$(SRC_DIR)/libguess.so: $(LIB_OBJ)
	$(CC) -shared -Wl,-soname,$@.1 -o $@.1.0 $^ -lc
	ln -sf $@.1.0 $@.1
	ln -sf $@.1 $@

.PHONY: clean help check

check:
	@$(foreach var,$(ALL_SOURCES),$(SRC_DIR)/checkpatch.pl --no-tree -f $(var);)

help:
	@echo "Usage: make or make LIB_TYPE=option"
	@echo "There are the next options:\n"
	@echo "\tstatic\tcreate and use static library\n"
	@echo "\tshared\tcreate and use shared library\n"
	@echo "By default no libraries are used"

clean:
	rm -f $(SRC_DIR)/*.o $(SRC_DIR)/*.a $(SRC_DIR)/*.so*
