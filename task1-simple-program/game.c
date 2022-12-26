#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//function to get a random namber in range min:max
int get_a_rand_num(int min, int max)
{
	srand(time(NULL));
	return min + rand() % (max - min + 1);
}

int main(void)
{
	//user_n - number is defined by user via console input
	//machine_n - number is difined randomly
	unsigned int user_n = 0, machine_n = 0;

	printf("Enter a number (0-9):\n");
	scanf("%u", &user_n);

	srand(time(NULL));
	machine_n = get_a_rand_num(0, 9);
	//printf("%u %u\n", machine_n, user_n);
	if (user_n == machine_n) {
		printf("You win\n");
		return 0;
	}
	printf("You loose\n");
	return 1;
}
