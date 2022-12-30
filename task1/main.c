// SPDX-License-Identifier: GPL-2.0
/*
 * (C) Copyright 2022
 * Author: Roman Kulchytskyi <kulch.roman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <stdio.h>

#include "utility.h"
#include "settings.h"

int main(void)
{
	printf("Enter number (%d - %d):\n", LOW, HIGH);
	const int userNumber = getUserNumber(LOW, HIGH);
	const int randomNumber = getRandomNumber(LOW, HIGH);

	if (userNumber == randomNumber) {
		printf("You guess the number %d!\n", userNumber);
		return 0;
	}

	printf("You lose. Number was %d!\n", randomNumber);
	return -1;
}
