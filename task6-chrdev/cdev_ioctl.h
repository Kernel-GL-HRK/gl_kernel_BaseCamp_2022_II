/* SPDX-License-Identifier: GPL */
#ifndef __CDEV_IOCTL_H__
#define	__CDEV_IOCTL_H__

#define	CDEV_IOC_MAGIC	'C'

enum cdev_ioctl {
	CLR_BUFFER_RETN = 1,
	GET_BUFFER_SIZE,
	SET_BUFFER_SIZE,
	CDEV_IOC_MAXNR,	/* keep last */
};

/* create command */
#define CDEV_CLR_VALUE  _IOR(CDEV_IOC_MAGIC, CLR_BUFFER_RETN, int32_t *)
#define CDEV_GET_VALUE	_IOR(CDEV_IOC_MAGIC, GET_BUFFER_SIZE, int32_t *)
#define CDEV_SET_VALUE	_IOW(CDEV_IOC_MAGIC, SET_BUFFER_SIZE, int32_t *)

#define BUFFER_SIZE	1024
#define CLASS_NAME	"chrdev"
#define DEVICE_NAME	"chrdev0"	/* /dev/chrdev0 */

#define CDEV_FILE	"/dev/"DEVICE_NAME""

#endif	/* __CDEV_IOCTL_H__ */
