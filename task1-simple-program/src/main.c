// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022. All rights reserved.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#include <time.h>

uint8_t generateRandomNumber()
{
	srand(time(NULL)); // Seed
	return rand() % 10;
}

int main(int argc, char **argv)
{
	uint8_t userNumber = 0;
	if (argc < 2){
		printf("Please gimme a number in range [0-9]");
		scanf("%hhd", &userNumber);
	}else
		userNumber = atoi(argv[1]); 
 
	if (userNumber < 0 || userNumber > 9) {
		printf("Value is not correct. Leaving\n");
		return -1;
	}

	if (userNumber == generateRandomNumber()) {
		//printf("You win!\n");
	} else {
		//printf("You lose\n");
		return -1;
	}

	return 0;
}

