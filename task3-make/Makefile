#************************************************************#
#							     #
#	Makefile					     #
#	by Yevhen Yefimov				     #
#							     #
#************************************************************#

NAME = game

#This flags will indicate all warnings during compilation
FLAGS = -Wall -Wextra -Werror

#Choose compile option here "gcc" or "clang"
CC = gcc

SRC = guess_a_number.c\
      random_number_function.c
OBJ = $(SRC:.c=.o)

#Choose the library what you need to add to compiling .a or .so
LIB = examplelib.a

#Set '-g' for debugging and "" for not
DEBUG = -g

all: $(NAME)

$(NAME): $(OBJ)
	@echo Compiling game . . .
	$(CC) $(FLAGS) $(DEBUG) $(OBJ) -o $(NAME) $(LIB)

%.o: %.c
	$(CC) -c $(FLAGS) $(DEBUG) -o $@ $<

#checkpatch.pl should be instaled to the /bin folder
check:
	checkpatch.pl --no-tree -f *.c *.h
