#include <stdio.h>

int main(void)
{
	char  buf[6000];
	FILE * fptr = fopen("/proc/mymod/mymod", "r");
	fgets(buf, 6000, fptr);
	puts(buf);
	fclose(fptr);
	return 0;
}
