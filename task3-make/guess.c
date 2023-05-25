#include <time.h>
#include <stdio.h>
#include "functions.h"

int main(void)
{
	int r = randm();
	int i;

	do {
		printf("Enter some number from 0 to 9\n");
		scanf("%d", &i);
	} while (i < 0 || i > 9);

	if (i == r) {
		printf("You win\n");
	} else {
		printf("You loose\n");
		return 1;
	}
	return 0;
}
