#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "randnum.h"

int randNumber(void) {
    int n;
  /* Intializes random number generator */
    srand(time(NULL));
    n = rand() % (9 + 1); // 0 - 9

    return n;
}
