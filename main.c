// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
/*
 * Game "Guess a Number"
 * User inputs a number from 0 to 9
 * Computer generates a random number from 0 to 9
 * If values are equal, program generates:
 *	- message "You win";
 *	- return code 0.
 * If values are not equal, program generates:
 *	- message "You loose";
 *	- return code 1.
 */

#include <stdio.h>
#include <time.h>

int main(void)
{
	int usrnum;	//User input number
	int randnum;	//Random number
	int retst;	//Return status

	printf("Input number from 0 to 9: ");
	scanf("%d", &usrnum);
	randnum = clock() % 10;
//	printf("%d | %d (usrnum | randnum)\n", usrnum, randnum);
	if (usrnum == randnum) {
		printf("You win\n");
		retst = 0;
	} else {
		printf("You loos\n");
		retst = 1;
	}
	return retst;
}
