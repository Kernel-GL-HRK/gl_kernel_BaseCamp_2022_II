#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int get_rand(){
	srand(time(NULL));
	return rand() % 10;
}

int main(void){
	
	return 0;
}

