#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	//user_n - number is defined by user via console input
	//machine_n - number is difined randomly
	unsigned int user_n = 0, machine_n = 0;

	printf("Enter a number (0-9):\n");
	scanf("%u", &user_n);

	srand(time(NULL));
	machine_n = rand() % 10;

	if (user_n == machine_n) {
		printf("You win\n");
		return 0;
	}
	printf("You loose\n");
	return 1;
}
