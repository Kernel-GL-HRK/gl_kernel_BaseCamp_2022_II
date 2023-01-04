#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int get_rand(){
	srand(time(NULL));
	return rand() % 10;
}

int main(void){
	
	int num;
	printf("Enter a number from 0 to 9: ");
	scanf("%d", &num);
	
	if (num >= 0 && num<= 9){
		if (num == get_rand()){
			printf("You win.\n");
			return 0;
		} else {
			printf("You loose.\n");
			return 1;
		}
		
	} else{
		printf("Wrong number, you loose.\n");
		return 2;
	}	
}

