#include <stdio.h>
#include <stdint.h>
#include "random.h"

int main() 
{
  uint16_t UserNum = 0xFFFF, QuessNum = 0xFFFF;

  printf("Please enter number from 0 to 9: ");
  scanf("%hd", &UserNum);
  if (UserNum <= 9) {
    QuessNum = randomize();
    if (UserNum == QuessNum) {
      printf("!!!You win!!!\n");
    } else{
      printf("!!!You lose!!! - Quess Number was: %d\n", QuessNum);
    }
  }
}
