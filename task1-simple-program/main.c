#include <stdio.h>
#include "guess.h"

int main(void)
{
	int user_val, gen_val;

	printf("Enter a number [0-9]: ");
	scanf("%d\n", &user_val);
	gen_val = get_rand();

	if (user_val == gen_val) {
		printf("You win\n");
		return 0;
	}

	printf("You loose\n");

	return -1;
}
