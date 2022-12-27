// SPDX-License-Identifier: GPL-2.0-or-later
/* Simple Guess a number program.
 *
 * Written by Yevhen Yefimov (mirexcool@gmail.com)
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

unsigned short get_random_number(void)
{
	srand(time(NULL)); // Seed
	return rand() % 10; // Generate random number form 0 to 9
}

int main(void)
{
	unsigned short randomNumber = 0;
	unsigned short userNumber = 0;

	randomNumber = get_random_number();
	printf("Enter the number beetwen 0 and 9:\n");
	scanf("%hu", &userNumber); // Read number from console
	if (userNumber != randomNumber) {
		printf("You loose\n");
		return 1;
	}
	printf("You win\n");
	return 0;
}
