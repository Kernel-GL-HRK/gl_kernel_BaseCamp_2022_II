#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	srand(time(NULL));
	puts("Enter number between 0 and 9");

	int input_number;

	scanf("%d", &input_number);
	if (input_number == rand() % 10) {
		puts("You win");
		return 0;
	}
	puts("You loose");
	return 1;
}
