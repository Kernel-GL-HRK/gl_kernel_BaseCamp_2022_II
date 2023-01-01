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
LIBS = $(BUILD)/libguess.a
OBJS = $(BUILD)/guess_game.o $(BUILD)/guess_lib.o

CC = gcc
CFLAGS = -Wall -Werror -g

.PHONY: format all clean check backup

# Format source code
format:
	$(CLANG_FORMAT) -i $(SRC)/*.[ch]

# Check of format source code
check:
	@echo "\n\e[32m*** Check format of source code ***\e[0m\n"
	@utils/checkpatch.pl --no-tree -f src/*.[ch]

backup: $(DIST)
	@echo "\n\e[32m*** Backup files ***\e[0m\n"
	tar -czvf $(DIST)/guess_game.tar.gz ./src/*


all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(BUILD)/guess_game.o $(LIBS)
	@echo "\n\e[31m*** Build target ***\e[0m\n"
	$(CC) $(CFLAGS) $(BUILD)/guess_game.o -L$(BUILD) -lguess -o $@

$(BUILD)/%.o: $(SRC)/%.c $(BUILD)
	$(CC) $(CFLAGS) -g -o $@ -c $<

$(LIBS): $(BUILD)/guess_lib.o
	ar rcs $@ $^

release: $(DIST) $(BUILD)/$(TARGET)
	@echo "\n\e[32m*** Create release distribution ***\e[0m\n"
	cp $(BUILD)/$(TARGET) $(DIST)/$(TARGET)

clean:
	@echo "\n\e[31m*** Cleaning dirs ***\e[0m\n"
	rm -rf $(BUILD)
	rm -rf $(DIST)

$(DIST):
	@echo "\n\e[33m*** Creating $(DIST) dir ***\e[0m\n"
	mkdir $(DIST)

$(BUILD):
	@echo "\n\e[33m*** Creating $(BUILD) dir ***\e[0m\n"
	mkdir $(BUILD)



