#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

#define CLASS_NAME	"servo"
#define DEVICE_NAME	"servo_control"

#define MAX_BUFFER_SIZE	256

int32_t create_devFS(void);
void remove_devFS(void);