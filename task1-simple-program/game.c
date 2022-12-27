#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// the function returns a random integer in range (min:max)
int getrand(int min, int max)
{
	srand(time(NULL));
	return min + rand() % (max - min + 1);
}

int main(void)
{
	int user_n, comp_n;
	printf("Enter a number (0-9):\t\n");
	scanf("%d", &user_n);
	comp_n = getrand(0, 10);
	printf("%d\n", comp_n);
	if(user_n != comp_n)
	{
		printf("You loose\n");
		return 1;
	}
	printf("You win\n");
	return 0;
}
