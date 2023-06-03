#ifndef __IOCTL_H__
#define __IOCTL_H__

#define CDEV_IOC_MAGIC 'V'

static int32_t ioctl_val;

enum cdev_ioctl 
{
	WR_VALUE,
	RD_VALUE,
	CDEV_IOC_MAXNR, /* keep last */
};

#define CDEV_WR_VALUE        _IOW(CDEV_IOC_MAGIC, WR_VALUE, int32_t*)
#define CDEV_RD_VALUE        _IOR(CDEV_IOC_MAGIC, RD_VALUE, int32_t*)

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

#endif