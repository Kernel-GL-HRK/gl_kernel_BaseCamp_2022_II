#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

#define CLASS_NAME	"ultrasound"
#define DEVICE_NAME	"ultrasound_control"

#define MAX_BUFFER_SIZE	256

extern dev_t MM;
extern struct cdev device;
extern struct class *class_folder;
extern struct device *dev_file;
extern struct file_operations dev_fs;

int32_t create_devFS(void);
void remove_devFS(void);