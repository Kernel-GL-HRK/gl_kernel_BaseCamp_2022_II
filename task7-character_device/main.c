#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "cdev_ioctl.h"

int main(int argc, char *argv[])
{
	int fd;
	int32_t get_buffer_size, set_buffer_size;
	int option = 4;

	fd = open(CDEV_FILE, O_RDWR);
	if (fd < 0) {
		printf("Cannot open device file... %d\n", fd);
		return -1;

	}
	if (argc > 1) {
		option = atoi(argv[1]);
	} else {
		printf("Choose an option:\n1 - buffer clear\n2 - get buffer size\n3 - set buffer size\n4 - exit\n");
		scanf("%d", &option);
	}
	switch (option) {
	case 1:
		printf("Clear buffer\n");
		ioctl(fd, CDEV_CLEAR_BUFFER);
		break;
	case 2:
		printf("Reading the buffer size\n");
		ioctl(fd, CDEV_GET_BUFFER_SIZE, (int32_t *) &get_buffer_size);
		printf("Buffer size is %d\n", get_buffer_size);
		break;
	case 3:
		printf("Enter the value to set buffer size\n");
		scanf("%d", &set_buffer_size);
		ioctl(fd, CDEV_SET_BUFFER_SIZE, (int32_t *) &set_buffer_size);
		break;
	default:
		break;

	}

	close(fd);
	return 0;
}
