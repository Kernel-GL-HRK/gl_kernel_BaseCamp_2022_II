#ifndef __cdev_ioctl_h__
#define __cdev_ioctl_h__

#define CDEV_IOC_MAGIC 'J'


enum chardev_ioctl {
	CLEAR_BUFFER,
	GET_FONT_SIZE,
	SET_FONT_SIZE,
	GET_TEXT_COLOR,
	SET_TEXT_COLOR,
	GET_BG_COLOR,
	SET_BG_COLOR,
	CDEV_IOC_MAXNR
};

#define CDEV_CLEAR_BUFFER _IO(CDEV_IOC_MAGIC, CLEAR_BUFFER)
#define CDEV_GET_FONT_SIZE _IOR(CDEV_IOC_MAGIC, GET_FONT_SIZE, int16_t*)
#define CDEV_SET_FONT_SIZE _IOW(CDEV_IOC_MAGIC, SET_FONT_SIZE, int16_t*)

#define CDEV_GET_TEXT_COLOR _IOR(CDEV_IOC_MAGIC, GET_TEXT_COLOR, int16_t*)
#define CDEV_SET_TEXT_COLOR _IOW(CDEV_IOC_MAGIC, SET_TEXT_COLOR, int16_t*)

#define CDEV_GET_BG_COLOR _IOR(CDEV_IOC_MAGIC, GET_BG_COLOR, int16_t*)
#define CDEV_SET_BG_COLOR _IOW(CDEV_IOC_MAGIC, SET_BG_COLOR, int16_t*)


#define CLASS_NAME "st7735"
#define DEVICE_NAME "chardev_st7735"

#define CDEV_FILE "/dev/"CLASS_NAME"0"

#endif /* __cdev_ioctl_h__ */
