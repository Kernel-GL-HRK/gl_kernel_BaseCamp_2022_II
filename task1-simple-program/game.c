#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(void)
{
	int user_n, comp_n;
	printf("Enter a number (0-9):\t\n");
	scanf("%d", &user_n);
	srand(time(NULL));
	comp_n = rand() % 10;
	printf("%d\n", comp_n);
	if(user_n != comp_n)
	{
		printf("You loose\n");
		return 1;
	}
	printf("You win\n");
	return 0;
}
