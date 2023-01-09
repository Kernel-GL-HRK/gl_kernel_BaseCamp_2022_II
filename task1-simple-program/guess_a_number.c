// SPDX-License-Identifier: GPL-3.0

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

int main(void)
{
	int input_num, rand_num;

	srand(time(NULL));
	rand_num = rand() % 10;
	printf("Guess a number from 0 to 9\n");

	//Uncomment for game debugging
	printf("rand_num: %d\n", rand_num);

	while (1) {
		printf("Enter your guess: ");
		if (!(scanf("%d", &input_num) > 0)) {
			printf("Not number at all. Try again!\n");
			getchar();//flush input stdint buffer to prevent infinite scanf reading
		} else {
			if (!(input_num >= 0 && input_num <= 9))
				printf("Number is out of range [0-9]. Try again!\n");
			else
				break;
		}
	}

	if (rand_num == input_num)
		printf("You win!\n");
	else
		printf("You loose(\nThe number was %d\n", rand_num);

	return 1;
}
