// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	int num, rdm;

	printf("Enter number from 0 to 9: ");
	scanf("%d", &num);
	// No range checks for input implemented

	srand(time(NULL));
	rdm = rand() % 10;

	printf("The number is %d\nYou entered %d\n", rdm, num);

	if (num != rdm) {
		puts("You lost!");
		exit(1);
	}

	puts("You win!");
	exit(0);
}
