// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
/*
 * Game "Guess a Number"
 * User inputs a number from 0 to 9
 * Computer generates a random number from 0 to 9
 * If values are equal, program generates:
 *	- message "You win";
 *	- return code 0.
 * If values are not equal, program generates:
 *	- message "You loose";
 *	- return code 1.
 */

#include <stdio.h>
#include <dlfcn.h>

int main(void)
{
	int usrnum;	//User input number
	int retst;	//Return status
	int rand_num;	//Random number

	printf("Input number from 0 to 9: ");
	scanf("%d", &usrnum);

	void *handle = dlopen("./librand10.so", RTLD_LAZY);

	if (!handle)
		printf("Cannot load library file\n");

	int (*lib_rand10)();

	lib_rand10 = (int(*)())dlsym(handle, "rand10");
	if (dlerror() != NULL)
		printf("Function rand10() not found\n");

	rand_num = (*lib_rand10)();
	dlclose(handle);

	if (usrnum == rand_num) {
		printf("You win\n");
		retst = 0;
	} else {
		printf("You loos\n");
		retst = 1;
	}
	return retst;
}
