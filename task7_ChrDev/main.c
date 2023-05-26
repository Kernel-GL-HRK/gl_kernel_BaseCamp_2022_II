// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
/*
 * Userspace program for kernel module
 * Character device example with SysFS and ioctl
 */

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
//#include <linux/ioctl.h>

#include "cdev_ioctl.h"

int main(void)
{
	int fd;
	int32_t value, number;

	printf("\nOpening Driver\n");
	fd = open(CDEV_FILE, O_RDWR);
	if (fd < 0) {
		printf("Cannot open device file...\n");
		return 0;
	}

	printf("Set the size of the buffer\n");
	scanf("%d", &number);
	printf("Writing Value to driver\n");
	ioctl(fd, CDEV_WR_VALUE, (int32_t *) &number);

	printf("Reading the size of the buffer\n");
	ioctl(fd, CDEV_RD_VALUE, (int32_t *) &value);
	printf("Buffer size is %d\n", value);

	printf("Do you want to clear  the buffer? (1/0)\n");
	scanf("%d", &number);
	if (number) {
		number = 0;
		ioctl(fd, CDEV_CL_VALUE, (int32_t *) &number);
	}
	printf("Closing driver\n");
	close(fd);
}
