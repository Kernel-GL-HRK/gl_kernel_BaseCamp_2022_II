#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ioctl_cmd.h"

enum{
	DMESG = 1,
	DEV,
	ALL,
};

int main(void)
{
	int fd;
	uint32_t choice = 0;

	fd = open("/dev/Matrix", O_RDWR);
	if (fd < 0) {
		printf("file is not opened");
		return 0;
	}

	printf("The menu for choice output of calculator result:\n");
	printf("\t1 - DMESG\n\t2 - /dev/Matrix\n\t3 - Everywhere\n");
	scanf("%d", &choice);
	printf("%d", choice);
	switch (choice) {
	case DMESG:
		printf("matrix-ioctl: result output will be desplayed in dmesg\n");
		ioctl(fd, OUTPUT_TO_DMESG, NULL);
		break;
	case DEV:
		printf("matrix-ioctl: result output will be desplayed in dev/Matrix\n");
		ioctl(fd, OUTPUT_TO_DEV, NULL);
		break;
	case ALL:
		printf("matrix-ioctl: result output will be desplayed everywhere\n");
		ioctl(fd, OUTPUT_TO_EVERYWHERE, NULL);
		break;
	default:
		printf("Wrong choice!!!\n");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}