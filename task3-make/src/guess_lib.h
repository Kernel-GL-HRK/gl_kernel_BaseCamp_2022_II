/* SPDX-License-Identifier: GPL-2.0 */

#ifndef GUESS_LIB
#define GUESS_LIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int get_user_input(char *message, char *variants);
int get_comp_input(void);
int guess_game(void);

#endif
