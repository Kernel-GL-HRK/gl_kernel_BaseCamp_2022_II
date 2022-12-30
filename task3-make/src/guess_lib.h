#ifndef GUESS_LIB
#define GUESS_LIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern const char WinMessage[];
extern const char LooseMessage[];

int get_user_input(char *message, char *variants);
int get_comp_input(void);
int guess_game(void);

#endif
