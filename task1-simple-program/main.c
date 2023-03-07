#include <stdio.h>
#include "func.h"

static const unsigned char upper = 9;

int main(void)
{
	unsigned char user_choice;

	printf("Guess a number (0-%hhd) -> ", upper);

	if (!scanf("%d", &user_choice) || user_choice < 0 || user_choice > upper) {
		printf("Wrong input\n");
		return -1;
	}

	const unsigned char pc_choice = get_random_number(0, upper);

	if (user_choice != pc_choice) {
		printf("You lost %d != %d\n", user_choice, pc_choice);
		return 1;
	}

	printf("You won %d = %d\n", user_choice, pc_choice);
	return 0;
}
