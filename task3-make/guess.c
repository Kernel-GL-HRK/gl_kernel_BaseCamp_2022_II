#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "guess.h"

void generateRandom(void)
{
	srand(time(0));
	return rand()%10;
}
