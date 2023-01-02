// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"

static int generate_number(void)
{
	srand(time(NULL));
	return rand() % 10;
}

void run_game(void)
{
	int num, rdm;

	printf("Enter number from 0 to 9: ");
	scanf("%d", &num);

	rdm = generate_number();
	printf("The number is %d\nYou entered %d\n", rdm, num);

	if (num != rdm) {
		puts("You lost!");
		exit(1);
	}

	puts("You win!");
	exit(0);
}
