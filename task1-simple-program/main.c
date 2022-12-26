#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int get_random_number(int lower, int upper) {
	return rand() % (upper - lower + 1) + lower;
}

int main() {
	int input;

	printf("Guess a number -> ");

	if (!scanf("%d", &input)) {
		printf("Wrong input\n");
		return -1;
	}

	srand(time(NULL));

	const int random = get_random_number(0, 9);

	if (input == random) {
		printf("You win %d = %d\n", input, random);
		return 0;
	}
	else {
		printf("You loose %d != %d\n", input, random);
		return 1;
	}
}
