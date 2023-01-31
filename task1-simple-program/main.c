#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

int main() {
  uint16_t UserNum = 0xFF, QuessNum = 0xFF;
  srand(time(NULL));
  printf("Please enter number from 0 to 9: ");
  scanf("%d", &UserNum);
  if (UserNum <= 9) {
    QuessNum = rand() % 10;
    if (UserNum == QuessNum)
      printf("!!!You win!!!");
    else
      printf("!!!You lose!!! - Quess Number was: %d", QuessNum);
  }
} 