#************************************************************#
#							     #
#	Makefile					     #
#	by Yevhen Yefimov				     #
#							     #
#************************************************************#

NAME = game
FLAGS = -Wall -Wextra -Werror
#Chhose compile option here "gcc" or "clang"
CC = clang

SRC = guess_a_number.c\
      random_number_function.c
OBJ = $(SRC:.c=.o)


all: $(NAME)

$(NAME): $(OBJ)
	@echo Compiling game . . .
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) -c $(FLAGS) -o $@ $<
