// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
/*
 * Return random number from 0 to 9
 * Use two-factor randomization:
 *	- Epoch time, measured in seconds
 *	- processor clock (from the beginning of the program)
 */

#include <time.h>

int rand10(void)
{
	int randnum;

	randnum = (time(NULL) + clock()) % 10;
	return randnum;
}
