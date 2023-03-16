#include <time.h>
#include <stdlib.h>

int get_rand(void)
{
	srand(time(NULL));
	return rand() % 10;
}
