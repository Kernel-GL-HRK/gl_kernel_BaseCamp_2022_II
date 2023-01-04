// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int main(void)
{
	int user_number, computer_number;
	bool result;

	printf("Enter your number from 0 to 9: ");
	scanf("%d", &user_number);

	if (user_number >= 0 && user_number <= 9) {
		srand(time(NULL));
		computer_number = rand() % 10;

		//printf("Computer number: %d\n", computer_number);

		result = (user_number == computer_number);
		printf("You %s\n", result ? "win" : "loose");
	} else {
		printf("incorrect number\n");
		result = 0;
	}

	return !(int)(result);
}
