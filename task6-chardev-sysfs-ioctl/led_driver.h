#ifndef __led_driver_h__
#define __led_driver_h__

#define IOCTL_MAGIC	"W"

enum drv_ioctl {
	WR_DELAY = 1,
	RD_DELAY,
	IOCTL_MAXN	/* last cmd */
};

#define LED_WR_DELAY	_IOW(IOCTL_MAGIC, WR_DELAY, int32_t *)
#define LED_RD_DELAY	_IOR(IOCTL_MAGIC, RD_DELAY, int32_t *)

#define DEV_CLASS	"led_class"
#define DEV_NAME	"led_device"

#define DEV_FILE	"/dev/"DEV_NAME

#endif /* __led_driver_h__ */
