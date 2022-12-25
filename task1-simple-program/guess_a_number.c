// SPDX-License-Identifier: GPL-2.0-or-later
/* Simple Guess a number program.
 *
 * Written by Yevhen Yefimov (mirexcool@gmail.com)
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(void)
{
	srand(time(NULL)); // Seed
	unsigned short randomNumber = rand() % 10; // Generate random number form 0 to 9
	unsigned short userNumber = 0;

	printf("Enter the number beetwen 0 and 9:\n");
	scanf("%hu", &userNumber); // Read number from console
	if (userNumber != randomNumber) {
		printf("You loose\n");
		return 1;
	}
	printf("You win\n");
	return 0;
}
