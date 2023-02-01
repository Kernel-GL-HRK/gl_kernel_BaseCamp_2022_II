#include "random.h"
#include "stdio.h"
#include "stdint.h"
#include <stdlib.h>
#include <time.h>

uint16_t randomize(void)
{
  srand(time(NULL));
  return (rand() % 10);

}