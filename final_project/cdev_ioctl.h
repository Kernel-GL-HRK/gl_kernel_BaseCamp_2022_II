// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
/*
 * ioctl for kernel module
 * Character device keypad_4x4
 */

#ifndef __cdev_ioctl_h__
#define __cdev_ioctl_h__

#define CDEV_IOC_MAGIC 'V'
enum cdev_ioctl {
	WR_VALUE = 1,
	RD_VALUE,
	CL_VALUE,
	CDEV_IOC_MAXNR, /* keep last */
};

#define CDEV_WR_VALUE _IOW(CDEV_IOC_MAGIC, WR_VALUE, int32_t*)
#define CDEV_RD_VALUE _IOR(CDEV_IOC_MAGIC, RD_VALUE, int32_t*)
#define CDEV_CL_VALUE _IOW(CDEV_IOC_MAGIC, CL_VALUE, int32_t*)

#define CLASS_NAME "chrdev"
#define DEVICE_NAME "chrdev_keypad"

#define CDEV_FILE "/dev/"CLASS_NAME"0"

#endif /* __cdev_ioctl_h__ */
