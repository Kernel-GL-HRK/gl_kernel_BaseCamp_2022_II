/*
 * This is a simple game "guess a number".
 * 03.01.2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define EXIT_SUCCSESS	0
#define EXIT_FAIL_1	1

int main(void)
{
	unsigned int random_number;
	unsigned int user_number;
	int exit_code = 0;

	srand((unsigned int) time(NULL));
	random_number = random() % 10;

	scanf("%d", &user_number);

	if (user_number == random_number) {
		printf("You win\n");
		exit_code = EXIT_SUCCSESS;
	} else {
		printf("You loose\n");
		exit_code = EXIT_FAIL_1;
	}

	return exit_code;
}
