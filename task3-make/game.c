// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"

#define EGG 42

static inline int generate_number(void)
{
	return rand() % 10;
}

void run_game(void)
{
	int num, rdm;

	srand(time(NULL));

	printf("Enter number from 0 to 9: ");
	scanf("%d", &num);

	rdm = generate_number();
	printf("The number is %d\nYou entered %d\n", rdm, num);

	// Let's add easter egg to the game
	if (num != rdm && num != EGG) {
		puts("You lost!");
		exit(1);
	}

	puts("You win!");
	exit(0);
}
