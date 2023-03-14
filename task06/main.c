#include <stdio.h>
#include <unistd.h>
int main(void)
{
	char  buf[6000];
	FILE * fptr = fopen("/dev/mymod", "r");
	sleep(15);
	fgets(buf, 6000, fptr);
	puts(buf);
	fclose(fptr);
	return 0;
}
