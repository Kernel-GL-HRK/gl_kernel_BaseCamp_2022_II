#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int get_random_number(int lower, int upper) {
	return rand() % (upper - lower + 1) + lower;
}

int main() {
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
	}
	else {
		printf("You loose %d != %d\n", input, random);
		return 1;
	}
}
