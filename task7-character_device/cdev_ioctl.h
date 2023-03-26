#ifndef __cdev_ioctl_h__
#define __cdev_ioctl_h__

#define CDEV_IOC_MAGIC 'J'

enum chardev_ioctl {
	CLEAR_BUFFER,
	GET_BUFFER_SIZE,
	SET_BUFFER_SIZE,
	CDEV_IOC_MAXNR
};

#define CDEV_CLEAR_BUFFER _IO(CDEV_IOC_MAGIC, CLEAR_BUFFER)
#define CDEV_GET_BUFFER_SIZE _IOR(CDEV_IOC_MAGIC, GET_BUFFER_SIZE, int32_t*)
#define CDEV_SET_BUFFER_SIZE _IOW(CDEV_IOC_MAGIC, SET_BUFFER_SIZE, int32_t*)

#define CLASS_NAME "chrdev"
#define DEVICE_NAME "chardev_device"

#define CDEV_FILE "/dev/"CLASS_NAME"0"

#endif /* __cdev_ioctl_h__ */
