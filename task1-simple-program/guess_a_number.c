#include <stdio.h>
#include "randomeGenerate.h"

int main()
{
	int userNumber, result;
	result = scanf("%d", &userNumber);

	if(userNumber == randomGenerate()){
		printf("You win\n");
		return 0;
	}
	else{
		printf("You loose\n");
		return 4;
	}
}
