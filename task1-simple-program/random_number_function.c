// SPDX-License-Identifier: GPL-2.0-or-later
/* random_number_function.c
 *
 * Written by Yevhen Yefimov (mirexcool@gmail.com)
 */

#include "guess_a_number.h"

unsigned int random_generator_function(void)
{
	srand(time(NULL)); // Seed
	return rand() % 10; // Return random number between 0 and 9
}
