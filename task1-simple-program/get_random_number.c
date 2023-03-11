#include <stdlib.h>
#include <time.h>
#include "get_random_number.h"

unsigned char get_random_number(unsigned char lower, unsigned char upper)
{
	static unsigned char randomized;
	if (!randomized) {
		srand(time(NULL));
		randomized = 1;
	}
	return rand() % (upper - lower + 1) + lower;
}
