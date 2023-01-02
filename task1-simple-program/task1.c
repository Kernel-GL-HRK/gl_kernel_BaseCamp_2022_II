// SPDX-License-Identifier: GPL-2.0-or-later
/* Random guess number.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_MIN  0
#define NUM_MAX  9
#define RETURN_FALSE   -1

unsigned short get_random(void)
{
 	 srand(time(NULL));
  	 return rand()%(NUM_MAX+1);
}

int main(void)

{
	int number_fromUser;


do	{
	printf("Please imput some number from 0 to 9: ");
	scanf("%d", &number_fromUser);
	}
while ((number_fromUser > NUM_MAX) || (number_fromUser < NUM_MIN));

if (get_random() == number_fromUser) {
	printf("You win\n");
	return 0;
	}
}
