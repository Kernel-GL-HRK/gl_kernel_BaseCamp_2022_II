#ifndef __encoder_ioctl_h__
#define __encoder_ioctl_h__

#define CDEV_IOC_MAGIC 'J'

enum encoder_ioctl {
	GET_ENCODER,
	SET_ENCODER,
	GET_DEBOUNCE_DELAY,
	SET_DEBOUNCE_DELAY,
	SET_PID,
	ENC_IOC_MAXNR
};

#define CDEV_GET_ENCODER _IOR(CDEV_IOC_MAGIC, GET_ENCODER, int32_t*)
#define CDEV_SET_ENCODER _IOW(CDEV_IOC_MAGIC, SET_ENCODER, int32_t*)

#define CDEV_GET_DEBOUNCE_DELAY _IOR(CDEV_IOC_MAGIC, GET_DEBOUNCE_DELAY, int32_t*)
#define CDEV_SET_DEBOUNCE_DELAY _IOW(CDEV_IOC_MAGIC, SET_DEBOUNCE_DELAY, int32_t*)

#define CDEV_SET_PID _IOW(CDEV_IOC_MAGIC, SET_PID, pid_t*)

#define CLASS_NAME "encoder_driver"
#define DEVICE_NAME "char_encoder_device"

#define CDEV_FILE "/dev/"CLASS_NAME"0"

#endif /* __encoder_ioctl_h__ */
