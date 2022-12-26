#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
	int input;

	printf("Guess a number -> ");

	if (!scanf("%d", &input)) {
		printf("Wrong input\n");
		return -1;
	}

	srand(time(NULL));

	const int random = rand() % 10;

	if (input == random) {
		printf("You win %d = %d\n", input, random);
		return 0;
	}
	else {
		printf("You loose %d != %d\n", input, random);
		return 1;
	}
}
