// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdlib.h>
#include <time.h>
#include "get_computer_number.h"

int get_computer_number(void)
{
	srand(time(NULL));
	return (rand() % 10);
}

