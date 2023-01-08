// SPDX-License-Identifier: GPL-2.0+
#include <stdio.h>
#include "func.h"

int main(void)
{
int x, x_gues, ret_val;

printf("Enter your number in the range from 0 to 9\n");
scanf("%u", &x_gues);
x = range_rand(0, 9);
if (x == x_gues) {
	printf("You Win!\n");
	ret_val = 0;
} else {
	printf("You Lose\nThe lucky number was %d\n", x);
	ret_val = -1;
}
return (ret_val);
}
