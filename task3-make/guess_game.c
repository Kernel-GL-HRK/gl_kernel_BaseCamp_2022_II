
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "random.h"


int main(int argc, char *argv[])
{
	srand(time(NULL));
	uint8_t randNum = get_random();
	uint8_t userNum = 0;

	if (argc > 1) {
		userNum = atoi(argv[1]);
	}

	else {
		printf("Enter your number (0-9): ");
		scanf("%hhu", &userNum);
	}

	if (userNum != randNum) {
		printf("You loose (%hu - %hu)\n", userNum, randNum);
		return -1;
	}

	printf("You win (%hu - %hu)\n", userNum, randNum);
	return 0;
}
