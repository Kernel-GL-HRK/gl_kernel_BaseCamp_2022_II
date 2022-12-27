#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static const int LOW; // default to 0
static const int HIGH = 9;

// generates random number in range [low, high]
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

// get user number in range [low, high]
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

int main()
{
    printf("Enter number (%d - %d):\n", LOW, HIGH);
    const int userNumber = getUserNumber(LOW, HIGH);
    const int randomNumber = getRandomNumber(LOW, HIGH);
    if (userNumber == randomNumber) {
        printf("You guess the number %d!\n", userNumber);
        return 0;
    } else {
        printf("You lose. Number was %d!\n", randomNumber);
        return -1;
    }

}
