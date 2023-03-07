#include <stdlib.h>
#include <time.h>
#include "func.h"

static char randomized;

int get_random_number(int lower, int upper)
{
	if (!randomized) {
		srand(time(NULL));
		randomized = 1;
	}
	return rand() % (upper - lower + 1) + lower;
}
