#include "func.h"

#include <time.h>

static char randomized;

int get_random_number(int lower, int upper)
{
	if (!randomized) {
		srand(time(NULL));
		randomized = 1;
	}
	return rand() % (upper - lower + 1) + lower;
}
