// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022. All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

int main(int argc, char **argv)
{
	uint8_t userNumber = 0;

	printf("Please gimme a number in range [0-9]");
	scanf("%hhd", &userNumber);

	if (userNumber < 0 || userNumber > 9) {
		printf("Value is not correct. Leaving\n");
		return -1;
	}

	if (userNumber == generateRandomNumber()) {
		printf("You win!\n");
	} else {
		printf("You lose\n");
		return -1;
	}

	return 0;
}

