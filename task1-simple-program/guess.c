#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	int user_val, gen_val;

	srand(time(NULL));
	printf("Enter a number [0-9]: ");
	scanf("%d\n", &user_val);
	gen_val = rand() % 10;

	if (user_val == gen_val) {
		printf("You win\n");
		return 0;
	}

	printf("You loose\n");

	return -1;
}
