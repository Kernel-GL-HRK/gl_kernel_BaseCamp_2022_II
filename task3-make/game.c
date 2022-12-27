#include <stdio.h>

#define DYNLINK

#ifdef DYNLINK
#include <dlfcn.h>
#endif

#ifndef DYNLINK
#include "func.h"
#endif

int main(void)
{
	int user_n, comp_n;

	printf("Enter a number (0-9):\t\n");
	scanf("%d", &user_n);
	#ifdef DYNLINK
		void* lib_handle;       /* handle of the opened library */
		lib_handle = dlopen("libfunc.so", RTLD_LAZY);
		if (!lib_handle) {
    			printf("Error during dlopen(): %s\n", dlerror());
    			return -1;
		}
		int (*getrand)(int, int);
		getrand = (int (*)(int, int))dlsym(lib_handle,"getrand");
		if(dlerror() != NULL) {
                        printf("Error during dlsym(): %s\n", dlerror());
                        return -1;
                }
		comp_n = getrand(0, 9);
	#endif
	#ifndef DYNLINK
		comp_n = getrand(0, 9);
	#endif
	printf("%d\n", comp_n);
	if (user_n != comp_n) {
		printf("You loose\n");
		return 1;
	}
	printf("You win\n");
	return 0;
}
