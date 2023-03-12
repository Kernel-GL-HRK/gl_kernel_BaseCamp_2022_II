#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kostiantynmakhno@gmail.com");
MODULE_DESCRIPTION("Simple character device driver");
MODULE_VERSION("1.0");

#define MAX_BUFF_LEN 1024UL

struct driver_data {
	struct class *pclass;
	struct device *pdevice;
	dev_t dev_num;
	struct device_data dev_data;
};

struct device_data {
	cdev dev;
	size_t buff_len;
	char *buff;
};

static char chrdev_buff[MAX_BUFF_LEN];

static struct driver_data drv_data = {
	.dev_data = {
		.buff_len = MAX_BUFF_LEN,
		.buff     = chrdev_buff
	}
};

static const file_operations fops = {
	.owner   = THIS_MODULE,
	.open    = chrdev_open,
	.release = chrdev_release,
	.read    = chrdev_read,
	.write   = chrdev_write
};

static int __init chrdev_init(void)
{
	return 0;
}

static void __exit chrdev_exit(void)
{

}

module_init(chrdev_init);
module_exit(chrdev_exit);
