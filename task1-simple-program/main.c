#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"

int randNumber(void);

int main(int argc, char **argv) {

    int a, rand_num, var;

    rand_num = randNumber();

    printf("%s * %s%s        GAME \"Guess number!\"        %s\n",ON_IBLACK ,RESET ,ON_IBLUE ,RESET);
    printf("%s * %s%s        guess the number *%sN%s%s*        %s\n",ON_IBLACK ,RESET ,ON_IBLUE ,BIWHITE, RESET, ON_IBLUE, RESET);


return 0;
}

int randNumber(void) {
    int n;

  /* Intializes random number generator */
    srand(time(NULL));
    n = rand() % (9 + 1); // 0 - 9

    return n;
}