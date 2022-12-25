#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int user_val, gen_val, cmp;

	scanf("%d\n", &user_val);

	gen_val = rand() % 10;
	cmp = (user_val == gen_val);
	printf("You %s\n", cmp ? "win" : "loose");
	
	if (!cmp)
		return -1;

	return 0;
}
