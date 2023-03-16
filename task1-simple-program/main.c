#include <stdio.h>
#include "get_random_number.h"

static const unsigned char upper = 9;

int main(void)
{
	unsigned char user_choice;

	printf("Guess a number (0-%hhd) -> ", upper);

	if (!scanf("%hhd", &user_choice) || user_choice > upper) {
		printf("Wrong input\n");
		return -1;
	}

	const unsigned char pc_choice = get_random_number(0, upper);

	if (user_choice != pc_choice) {
		printf("You lost %hhd != %hhd\n", user_choice, pc_choice);
		return 1;
	}

	printf("You won %hhd = %hhd\n", user_choice, pc_choice);
	return 0;
}
