#include <stdio.h>
#include "guess.h"

int main(void)
{
    int guestNumber;
    printf("Welcome to Guess a number!\nEnter your number (0-9):");
    scanf("%d", &guestNumber);
    compNumber = generateRandom();

    if(compNumber==guestNumber)
    {
        printf("You win!\n");
        return 0;
    }
    else
    {
        printf("You loose!\n");
        return 1;
    }
}
