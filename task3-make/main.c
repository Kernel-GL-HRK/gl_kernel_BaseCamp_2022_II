/*
 * This is a simple game "guess a number".
 * 05.01.2023
 */

#include <stdio.h>
#include "random.h"

#define EXIT_SUCCSESS	0
#define EXIT_FAIL_1	1
#define EXIT_FAIL_2	2

int main(void)
{
	unsigned int random_number;
	int user_number;
	int exit_code = 0;

	random_number = random_gen();
	printf("Please enter the number from 0 to 9: ");
	scanf("%d", &user_number);

	if ((user_number < 0) || (user_number > 9)) {
		fprintf(stderr, "The number must be in the range from 0 to 9\n");
		exit_code = EXIT_FAIL_2;
	} else {
		if ((unsigned int)user_number == random_number) {
			printf("You win\n");
			exit_code = EXIT_SUCCSESS;
		} else {
			printf("You loose\n");
			exit_code = EXIT_FAIL_1;
		}
	}

	return exit_code;
}

