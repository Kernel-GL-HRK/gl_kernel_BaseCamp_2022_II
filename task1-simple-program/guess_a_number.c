#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NUMBER 9
#define MIN_NUMBER 0

static inline uint8_t generate_number(void)
{
	return (uint8_t)(rand() % (MAX_NUMBER - MIN_NUMBER + 1) + MIN_NUMBER);
}

int main(void)
{
	srand((unsigned int)time(NULL));

	return 0;
}
