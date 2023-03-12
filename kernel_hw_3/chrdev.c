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
#define DRV_CLASS_NAME "chrdev"
#define DRV_DEVICE_NAME "chrdev0"
#define DRV_PROC_NAME  "chrdev"

struct device_data {
	struct cdev dev;
	size_t buff_len;
	char *buff;
};

struct driver_data {
	struct proc_dir_entry *pfile;
	struct class *pclass;
	struct device *pdevice;
	dev_t dev_num;
	struct device_data dev_data;
};

static char chrdev_buff[MAX_BUFF_LEN];

static struct driver_data drv_data = {
	.dev_data = {
		.buff_len = MAX_BUFF_LEN,
		.buff     = chrdev_buff
	}
};

ssize_t chrdev_read(struct file *filp, char __user *ubuf, size_t count, loff_t *off)
{
	return 0;
}

ssize_t chrdev_write(struct file *filp, const char __user *ubuf, size_t count, loff_t *off)
{
	return 0;
}

int chrdev_open(struct inode *inode, struct file *filp)
{
	return 0;
}

int chrdev_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static const struct file_operations fops = {
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
