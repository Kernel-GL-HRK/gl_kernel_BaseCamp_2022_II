#include "utility.h"

#include "settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int getRandomNumber(int low, int high)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (low < high) {
        low_num = low;
        hi_num = high + 1; // include max_num in output
    } else {
        low_num = high + 1; // include max_num in output
        hi_num = low;
    }

    srand(time(NULL));
    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}

int getUserNumber(int low, int high)
{
    int userNumber = -1;

    do {
        scanf("%d", &userNumber);
        if (userNumber < low || userNumber > high) {
            printf("Number should be in range [%d, %d]\n", LOW,
                   HIGH);
            userNumber = -1;
        }
    } while (userNumber == -1);

    return userNumber;
}
