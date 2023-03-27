// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void generateRandom(void)
{
	srand(time(0));
	int compNumber = rand()%10;
}

int main(void)
{
	int guestNumber;

	printf("Welcome to Guess a number!\nEnter your number (0-9):");
	scanf("%d", &guestNumber);
	generateRandom();

	if (compNumber == guestNumber) {
		printf("You win!\n");
	} else {
		printf("You loose!\n");
		return 1;
	}
	return 0;
}