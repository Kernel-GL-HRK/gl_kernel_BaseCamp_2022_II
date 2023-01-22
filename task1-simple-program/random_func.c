// SPDX-License-Identifier: GPL-2.0-or-later
/* Random guess number randomizer func
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "task1.h"

unsigned short get_random(void)
{
	srand(time(NULL));
	return rand()%(NUM_MAX+1);
}
