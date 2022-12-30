#include <stdio.h> 
#include "guess_lib.h"

int main(){
  printf("Results of input %i\n",get_user_input("Enter yn","yYnN"));
  
  return guess_game();

return 0;
}
