/* SPDX-License-Identifier: GPL */
#ifndef __chardev_ioctl__
#define __chardev_ioctl__

#define SOME_MAGIC 'K'

#define CLASS_NAME  "my_chardev"
#define DEVICE_NAME "my_chardev"

#define CHARDEV_FILE "/dev/"CLASS_NAME

enum cdev_ioctl {
	CLEAR = 0,
	GET_SIZE,
	SET_SIZE,
	END_IO,
};

#define CDEV_CLEAR _IO(SOME_MAGIC, CLEAR)
#define CDEV_GET_SIZE _IOR(SOME_MAGIC, GET_SIZE, int32_t*)
#define CDEV_SET_SIZE _IOW(SOME_MAGIC, SET_SIZE, int32_t*)

#endif
