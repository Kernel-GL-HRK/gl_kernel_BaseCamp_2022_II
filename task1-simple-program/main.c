#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

uint16_t randomize(void)
{
  srand(time(NULL));
  return (rand() % 10);

}

int main() 
{
  uint16_t UserNum = 0xFFFF, QuessNum = 0xFFFF;

  printf("Please enter number from 0 to 9: ");
  scanf("%hd", &UserNum);
  if (UserNum <= 9) {
    QuessNum = randomize();
    if (UserNum == QuessNum) {
      printf("!!!You win!!!\n");
      return 0;
    } else{
      printf("!!!You lose!!! - Quess Number was: %hd\n", QuessNum);
      return 1;
    }
  }
}
