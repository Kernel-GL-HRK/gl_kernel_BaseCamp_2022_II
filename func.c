// SPDX-License-Identifier: GPL-2.0+
#include <stdlib.h>
#include <time.h>


int range_rand(int min, int max)
{
srand(time(NULL));
return (min + rand() % (max - min + 1));
}

