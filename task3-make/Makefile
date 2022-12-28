#************************************************************#
#							     #
#	Makefile					     #
#	by Yevhen Yefimov				     #
#							     #
#************************************************************#


NAME = game
FLAGS = -Wall -Wextra -Werror
CC = gcc

SRC = guess_a_number.c\
      random_number_function.c
OBJ = $(SRC:.c=.o)


all: $(NAME)

$(NAME): $(OBJ)
	@echo Compiling game . . .
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) -c $(FLAGS) -o $@ $<
