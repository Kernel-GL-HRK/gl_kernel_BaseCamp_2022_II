#include <stdio.h>
#include "functions.h"

int random(void)
{
	srand(time(NULL));
	return rand() % 10;
}
