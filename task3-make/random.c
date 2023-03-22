#include "random.h"

uint8_t generate_number(void)
{
	return (uint8_t)(rand() % (MAX_NUMBER - MIN_NUMBER + 1) + MIN_NUMBER);
}
