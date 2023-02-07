#include <stdlib.h>
#include <time.h>

int randomGenerate()
{
	srand(time(NULL));
	int randomNumber = rand()%9;
	return randomNumber;
}
