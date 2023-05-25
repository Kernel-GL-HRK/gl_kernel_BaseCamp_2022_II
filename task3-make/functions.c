#include <stdlib.h>
#include <time.h>
#include "functions.h"

int randm(void)
{
	srand(time(NULL));
	return rand() % 10;
}
