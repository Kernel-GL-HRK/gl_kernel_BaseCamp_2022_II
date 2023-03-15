#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "mymod_uapi.h"

int main(void)
{
	int fd;
	uint32_t size;
	printf("Openning dev file\n");
	fd = open(MYMOD_FILE, O_RDWR);
	if (fd < 0) {
		printf("Failed to open %s\n", MYMOD_FILE);
		return -1;
	}
	ioctl(fd, MYMOD_GET_BUFFER_SIZE, &size);
	printf("Buffer size: %d\n", size);
	
	size = 512;
	printf("Set buffer size: %u\n", size);
	ioctl(fd, MYMOD_SET_BUFFER_SIZE, size);
	
	ioctl(fd, MYMOD_GET_BUFFER_SIZE, &size);
	printf("Buffer size: %d\n", size);
	
	printf("Clear buffer\n");
	ioctl(fd, MYMOD_CLEAR_BUFFER);
	 
	/* to check last step use cat /dev/mymod */
	
	
	return 0;
	
}
