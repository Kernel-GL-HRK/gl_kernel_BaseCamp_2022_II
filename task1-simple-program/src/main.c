#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint8_t generateRandomNumber(int lower, int upper)
{
    return rand() % (upper - lower + 1) + lower;
} 

int main (int argc, char **argv)
{
    for(int i = 0; i < 10; i++) {
        printf("generated number is - %d\n", generateRandomNumber(0, 9));
    }
    
    return 0;
}

