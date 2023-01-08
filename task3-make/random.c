#include <stdlib.h>
#include <time.h>
#include "random.h"

unsigned int random_gen(void)
{
	srand((unsigned int) time(NULL));
	return random() % 10;
}
