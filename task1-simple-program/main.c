#include <stdio.h>

#include "func.h"

static const unsigned int upper = 9;
static const unsigned int lower = 0;

int main(void)
{	
	int user_choice;

	printf("Guess a number (%d-%d) -> ", lower, upper);

	if (!scanf("%d", &user_choice) || user_choice < lower || user_choice > upper) {
		printf("Wrong input\n");
		return -1;
	}

	const int pc_choice = get_random_number(lower, upper);

	if (user_choice == pc_choice) {
		printf("You win %d = %d\n", user_choice, pc_choice);
		return 0;
	} else {
		printf("You loose %d != %d\n", user_choice, pc_choice);
		return 1;
	}
}
