#include <time.h>
#include <stdlib.h>

//the function returns a random integer in range (min:max)
int getrand(int min, int max)
{
        srand(time(NULL));
        return min + rand() % (max - min + 1);
}

