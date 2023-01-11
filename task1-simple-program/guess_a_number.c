// SPDX-License-Identifier: GPL-2.0

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_VALUE 0
#define MAX_VALUE 9

int get_rand_number(int min, int max)
{
	srand((unsigned int)time(NULL));
	return (rand() % (max - min + 1)) + min;
}

int main(void)
{
	int user_number, random_number;

	random_number = get_rand_number(MIN_VALUE, MAX_VALUE);

	printf("Welcome to game \"guess a number\"!\n");
	printf("Enter number from %d to %d: ", MIN_VALUE, MAX_VALUE);
	scanf("%d", &user_number);
	while (user_number < MIN_VALUE || user_number > MAX_VALUE) {
		printf("Invalid value. Number must be from %d to %d. Try again: ", MIN_VALUE, MAX_VALUE);
		scanf("%d", &user_number);
	}

	if (user_number == random_number) {
		printf("You win\n");
		return 0;
	}

	printf("You loose\n");
	return 1;
}
