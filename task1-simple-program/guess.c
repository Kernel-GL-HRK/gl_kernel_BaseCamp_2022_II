#include <stdlib.h>
#include <time.h>
#include "guess.h"

int get_rand(void)
{
	srand(time(NULL));
	return rand() % 10;
}

