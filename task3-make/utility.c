#include "utility.h"

uint8_t generateRandomNumber()
{
	srand(time(NULL)); // Seed
	return rand() % 10;
}