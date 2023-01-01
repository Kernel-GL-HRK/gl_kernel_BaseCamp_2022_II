# clang-format command
CLANG_FORMAT = clang-format-12

# Source dir
SRC = src
# Build dir
BUILD = build

# distribution or archive dir
DIST = dist

# compiler option

TARGET = guess_game

OBJS = $(BUILD)/guess_game.o $(BUILD)/guess_lib.o

CC = gcc
CFLAGS = -Wall

.PHONY: format all clean check backup

# Format source code
format:
	$(CLANG_FORMAT) -i src/*.[ch]

# Check of format source code
check:
	utils/checkpatch.pl --no-tree -f src/*.[ch]

backup: $(DIST)
	tar -czvf $(DIST)/guess_game.tar.gz ./src/*


all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(OBJS)
	echo Build target
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(BUILD)/%.o: $(SRC)/%.c $(BUILD)
	$(CC) $(CFLAGS) -g -o $@ -c $<


release: $(DIST) $(BUILD)/$(TARGET)
	echo DIST
	cp $(BUILD)/$(TARGET) $(DIST)/$(TARGET)

clean:
	rm -rf $(BUILD)
	rm -rf $(DIST)

$(DIST):
	echo Creating $(DIST) dir
	mkdir $(DIST)

$(BUILD):
	echo Creating $(BUILD) dir
	mkdir $(BUILD)

