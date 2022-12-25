#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int generate(void)
{
	srand(time(NULL));
	return rand() % 10;
}

int main(void)
{
	int user_val, gen_val, cmp;

	scanf("%d\n", &user_val);

	gen_val = generate();
	cmp = (user_val == gen_val);
	printf("You %s\n", cmp ? "win" : "loose");

	if (!cmp)
		return -1;

	return 0;
}
