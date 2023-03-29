/* SPDX-License-Identifier: GPL */
#ifndef __chardev_ui__
#define __chardev_ui__

#include <fcntl.h>
#include <sys/ioctl.h>

#include "chardev_ioctl.h"

int file;

int cdev_open(void)
{
	file = open(CHARDEV_FILE, O_RDWR);
	return file;
}

void cdev_set_size(int32_t num)
{
	ioctl(file, CDEV_SET_SIZE, &num);
}

int cdev_get_size(void)
{
	int32_t num;
	ioctl(file, CDEV_GET_SIZE, &num);
	return num;
}

void cdev_clear(void)
{
	ioctl(file, CDEV_CLEAR, NULL);
}

void cdev_close(void)
{
	close(file);
}

#endif
