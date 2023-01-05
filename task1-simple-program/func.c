#include "func.h"

int get_random_number(int lower, int upper)
{
        return rand() % (upper - lower + 1) + lower;
}
