#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	srand(time(NULL));
	int randomNumber = rand()%9;

	int userNumber, result;
	result = scanf("%d", &userNumber);

	if(userNumber == randomNumber){
		printf("You win\n");
		return 0;
	}
	else{
		printf("You loose\n");
		return 4;
	}
}
