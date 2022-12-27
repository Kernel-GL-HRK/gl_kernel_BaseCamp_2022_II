// SPDX-License-Identifier: GPL-2.0+
#include <stdio.h>
#include <stdlib.h>
int get_random_numeric(int);

int main(void)
{
int user_choice;

printf("User guess [0-9]:");
user_choice = getchar() - '0';
if (user_choice < 0 || user_choice > 9) {
	puts("Bad input");
	return -1;
}

if (user_choice == get_random_numeric(user_choice)) {
	puts("You win");
	return 0;
}
puts("You loose");
return 1;
}


int get_random_numeric(int user_choice){
srand(user_choice);
return rand()%10;
}
