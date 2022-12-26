#include <stdio.h>

#define  dynlink

#ifdef dynlink
	#include <dlfcn.h>
#endif

#ifndef dynlink
	 #include "func.h"
#endif

int main(void)
{
	//user_n - number is defined by user via console input
	//machine_n - number is difined randomly
	unsigned int user_n = 0, machine_n = 0;

	printf("Enter a number (0-9):\n");
	scanf("%u", &user_n);
	#ifdef dynlink

		void *handle = dlopen("libfunc.so", RTLD_LAZY);
		if(!handle)
		{
			printf("Can't find libfunc.so\n");
			return -1;
		} 
		int (*get_a_rand_num)(int, int);

		get_a_rand_num = (int (*)())dlsym(handle,"get_a_rand_num");
   		if(dlerror() != NULL)
		{
			printf("Can't find libfunc.so\n");
		}

   		(*get_a_rand_num)(0, 9);
	#else
		machine_n = get_a_rand_num(0, 9);
	#endif
	//printf("%u %u\n", machine_n, user_n);

	if (user_n == machine_n) {
		printf("You win\n");
		return 0;
	}
	printf("You loose\n");
	return 1;
}
