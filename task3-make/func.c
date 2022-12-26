#include <stdlib.h>
#include <time.h>
#include "func.h"

//function to get a random namber in range min:max
int get_a_rand_num(int min, int max)
{
        srand(time(NULL));
        return min + rand() % (max - min + 1);
};

