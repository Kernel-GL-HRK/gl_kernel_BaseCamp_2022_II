
#include "random.h"

int get_random(void)
{
	srand(time(NULL));
	return rand() % 10;
}