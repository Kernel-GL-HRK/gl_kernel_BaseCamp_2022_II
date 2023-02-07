#include <stdio.h>
#include <dlfcn.h>

int main()
{
	
	int userNumber, result;
	result = scanf("%d", &userNumber);
	
	void *handle = dlopen("./../librandomeGenerate.so", RTLD_LAZY);
	if(!handle){
		printf("error with load dynamic library");
	}
	int (*lib_generate)();
	lib_generate = (void (*)())dlsym(handle, "randomGenerate");
	if(dlerror() != NULL){
		printf("error");
	}

	if(userNumber == (*lib_generate)()){
		printf("You win\n");
		return 0;
	}
	else{
		printf("You loose\n");
		return 4;
	}
	dlclose(handle);
}
