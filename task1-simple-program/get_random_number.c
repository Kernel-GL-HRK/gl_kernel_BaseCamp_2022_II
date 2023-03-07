#include <stdlib.h>
#include <time.h>
#include "get_random_number.h"

static unsigned char randomized;

unsigned char get_random_number(unsigned char lower, unsigned char upper)
{
	if (!randomized) {
		srand(time(NULL));
		randomized = 1;
	}
	return rand() % (upper - lower + 1) + lower;
}
