#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "chardev_ioctl.h"

int main(void)
{
	int fd;
	size_t get_size, set_size;

	printf("Opening device\n");
	fd = open(CHARDEV_FILE, O_RDWR);
	if (fd < 0) {
		printf("Failed to open device\n");
		return -1;
	}
	printf("Enter new buffer size\n");
	scanf("%d", &set_size);
	printf("Setting new buffer size\n");
	ioctl(fd, CHARDEV_SET_BUFFER_SIZE, &set_size);

	printf("Getting buffer size\n");
	ioctl(fd, CHARDEV_GET_BUFFER_SIZE, &get_size);
	printf("Buffer size: %d\n", get_size);

	printf("Clear buffer\n");
	ioctl(fd, CHARDEV_CLEAR_BUFFER);
	printf("Getting buffer size\n");
	ioctl(fd, CHARDEV_GET_BUFFER_SIZE, &get_size);
	printf("Buffer size: %d\n", get_size);

	close(fd);
	return 0;
}
