# clang-format command
CLANG_FORMAT = clang-format-12

# Source dir
SRC = src
# Build dir
BUILD = build

# release dir for archive
RELEASE_DIR = release/

# compiler option

TARGET = guess_game

OBJS = guess_game.o guess_lib.o

CC = gcc


.PHONY: format

# Format source code
format:
	$(CLANG_FORMAT) -i src/*.[ch]

check:
	utils/checkpatch.pl --no-tree -f src/*.[ch]

backup:
	echo TODO backup

release: guess_game
	echo TODO release

all: $(TARGET)
	echo TODO all

$(TARGET): $(OBJS)
	echo TODO

%.o: $(SRC)/%.c
	echo TODO

build:
	echo TODO build

clean:
	rm -f *.o

