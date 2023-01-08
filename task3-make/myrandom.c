#include "myrandom.h"

int gen_rand(int bottom, int top)
{
	return rand() % (top - bottom + 1) + bottom;
}
