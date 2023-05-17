#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "chrdev_ioctl.h"

int main(void)
{
	int fd, c, ret;
	size_t buff_len;
	char buff[32];
	char *pbuff = buff;

	printf("start program\n");
	fd = open(DEVICE, O_RDWR);
	if (fd < 0) {
		printf("Can not open device file %s\n", DEVICE);
		return -1;
	}

	printf("usage:\n\t\t"
		"enter c     --> to clear buffer\n\t\t"
		"enter g     --> to get current buffer length\n\t\t"
		"enter s len --> to set new buffer length with value len\n\t\t"
		"enter h     --> to help\n\t\t"
		"enter q     --> to exit\n");

	do {
		printf("$ ");
		switch (c = getchar()) {
		case 'c':
			if ((ret = ioctl(fd, CHRDEV_IOCCBUFF)) < 0) {
				printf("clear buffer command error\n");
				return ret;
			}
			printf("buffer is empty\n");
			getchar();
		break;

		case 'g':
			if ((ret = ioctl(fd, CHRDEV_IOCGBUFF_LEN, &buff_len)) < 0) {
				printf("get buffer length command error\n");
				return ret;
			}
			printf("current buffer length is %zu\n", buff_len);
			getchar();
		break;

		case 'h':
			printf("usage:\n\t\t"
				"enter c     --> to clear buffer\n\t\t"
				"enter g     --> to get current buffer length\n\t\t"
				"enter s len --> to set new buffer length with value len\n\t\t"
				"enter h     --> to help\n\t\t"
				"enter q     --> to exit\n");
			getchar();
		break;

		case 's':
			while ((c = getchar()) == ' ');
			*pbuff++ = c;
			while ((*pbuff++ = getchar()) != '\n');
			*pbuff = '\0';
			pbuff = buff;
			buff_len = strtoul(buff, NULL, 10);
			printf("want to set new buffer length to value %zu\n", buff_len);
			if (ret = ioctl(fd, CHRDEV_IOCSBUFF_LEN, &buff_len)) {
				printf("set buffer length command error\n");
				return ret;
			}	
		break;

		case 'q':
			printf("exit from program\n");
		break;

		default:
		break;
		}
	} while (c != EOF && c != 'q');

	close(fd);
	return 0;
}