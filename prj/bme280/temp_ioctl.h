#ifndef __cdev_ioctl_h__
#define __cdev_ioctl_h__

#define CDEV_IOC_MAGIC 'J'

enum temp_ioctl {
	GET_TEMP,
	TEMP_IOC_MAXNR
};

#define CDEV_GET_TEMP _IOR(CDEV_IOC_MAGIC, GET_TEMP, int32_t*)


#define CLASS_NAME "device_bme280"
#define DEVICE_NAME "char_dev_my"

#define CDEV_FILE "/dev/"CLASS_NAME"0"

#endif /* __cdev_ioctl_h__ */