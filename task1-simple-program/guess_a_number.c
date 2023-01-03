#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NUMBER 9
#define MIN_NUMBER 0

static inline uint8_t generate_number(void)
{
	return (uint8_t)(rand() % (MAX_NUMBER - MIN_NUMBER + 1) + MIN_NUMBER);
}

int main(void)
{
	srand((unsigned int)time(NULL));

	printf("Enter your number: ");
	scanf("%hhu", &user_number);

	if (user_number > MAX_NUMBER) {
		fprintf(stderr, "Number not between %d and %d\n",
			MIN_NUMBER, MAX_NUMBER);
		return 2;
	}

	return 0;
}
