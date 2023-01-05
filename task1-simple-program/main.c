#include <stdio.h>
#include <time.h>

#include "func.h"

int main(void)
{
	const int upper = 9;
	const int lower = 0;

	int input;

	printf("Guess a number (%d-%d) -> ", lower, upper);

	if (!scanf("%d", &input) || input < lower || input > upper) {
		printf("Wrong input\n");
		return -1;
	}

	srand(time(NULL));

	const int random = get_random_number(lower, upper);

	if (input == random) {
		printf("You win %d = %d\n", input, random);
		return 0;
	} else {
		printf("You loose %d != %d\n", input, random);
		return 1;
	}
}
