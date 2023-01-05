#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "randnum.h"


int main(int argc, char **argv) {

    int a, rand_num, var;

    rand_num = randNumber();

    printf("%s * %s%s        GAME \"Guess number!\"        %s\n",ON_IBLACK ,RESET ,ON_IBLUE ,RESET);
    printf("%s * %s%s        guess the number *%sN%s%s*        %s\n",ON_IBLACK ,RESET ,ON_IBLUE ,BIWHITE, RESET, ON_IBLUE, RESET);

    do {
        printf("%s * %s\n%s * %s     Enter an integer 0 - 9 : ",ON_IBLUE, RESET, ON_IBLUE, RESET);
        var = scanf("%d", &a); // enter from the keyboard.
        while ((getchar()) != '\n');
        if ((var != 1) ||
        ((a > 9) || (a < 0))) {
            var = 0;
            printf("%s * %s  %sPlease try again!%s\n", ON_IBLUE, RESET, RED_BOLD, RESET);
        }


return 0;
}
