// SPDX-License-Identifier: GPL-2.0-or-later
/* guess_a_number.c
 *
 * Written by Yevhen Yefimov (mirexcool@gmail.com)
 */

#include "guess_a_number.h"

int main(void)
{
	unsigned short randomNumber = 0;
	unsigned short userNumber = 0;

	randomNumber = random_generator_function();
	printf("Enter the number beetwen 0 and 9:\n");
	scanf("%hu", &userNumber); // Read number from console
	if (userNumber != randomNumber) {
		printf("You loose\n");
		return 1;
	}
	printf("You win\n");
	return 0;
}
