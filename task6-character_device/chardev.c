// SPDX-License-Identifier: GPL
#include <linux/init.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/proc_fs.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/ioctl.h>

#include "chardev_ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kushnir Vladyslav");
MODULE_DESCRIPTION("Pcharacter devace");
MODULE_VERSION("0.1");

//DEV
#define BUFFER_SIZE 1024

static struct class  *pclass;
static struct device *pdev;
static struct cdev   chrdev_cdev;
dev_t dev;

static int major;
static int is_open;

static int data_size;
static unsigned char data_buffer[BUFFER_SIZE];

//PROC
#define PROC_BUFFER_SIZE (BUFFER_SIZE + 100)

static char proc_buffer[PROC_BUFFER_SIZE];

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

#define PROC_FILE_NAME DEVICE_NAME
#define PROC_DIR_NAME (DEVICE_NAME"_dir")

static bool is_read;

//SYS
#define SYSFS_NAME DEVICE_NAME
#define SYSFS_ATTR_NAME DEVICE_NAME"_attr"

static struct kobject *chrdev_kobj;

//IOCTL
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	if (_IOC_TYPE(cmd) != SOME_MAGIC) {
		pr_err("chardev: SOME_MAGIC failed.\n");
		return -ENOTTY;
	}

	if (_IOC_NR(cmd) >= END_IO) {
		pr_err("char_dev: wrong command.\n");
		return -ENOTTY;
	}

	switch (cmd) {
		case CDEV_CLEAR:
			data_size = 0;
			data_buffer[0] = '\n';
			break;
		case CDEV_GET_SIZE:
			if (copy_to_user((int32_t *) arg, &data_size, sizeof(data_size))) {
				pr_err("chardev: write failed.\n");
				return -EFAULT;
			}
			break;
		case CDEV_SET_SIZE:
			if (*((int32_t *) arg) > BUFFER_SIZE)
				*((int32_t *) arg) = BUFFER_SIZE;
			
			if (copy_from_user(&data_size, (int32_t *) arg, sizeof(data_size))) {
				pr_err("chardev: read failed.\n");
				return -EFAULT;
			}
		default:
			pr_err("char_dev: wrong command.\n");
			return -ENOTTY;
	}
	return 0;
}

//DEV
static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("chardev: device already open.\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("chardev: device opened.\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("chardev: device closed.\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	if (len > data_size)
		len = data_size;

	if (copy_to_user(buffer, data_buffer, len)) {
		pr_err("chardev: copy_to_user failed.\n");
		return -EFAULT;
	}
	data_size = 0;
	data_buffer[0] = '\0';
	pr_info("chardev: %d bytes read.\n", len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	if (len > BUFFER_SIZE)
		len = BUFFER_SIZE;

	if (copy_from_user(data_buffer, buffer, len)) {
		pr_err("chardev: copy_from_user failes.\n");
		return -EFAULT;
	}

	data_size = len;
	pr_info("chardev: %d bytes written.\n", len);
	return len;
}

//PROC
static ssize_t proc_read(struct file *FIle, char __user *buffer, size_t Count, loff_t *offset)
{
	is_read = !is_read;
	if (!is_read)
		return 0;

	size_t size_to_copy  = sprintf(proc_buffer, "Size of buffer: %d.\nCapasity of buffer: %d.\nBuffer contains:\n%s\n", data_size, BUFFER_SIZE, data_buffer);

	if (size_to_copy > Count)
		size_to_copy = Count;

	size_t not_copy = copy_to_user(buffer, proc_buffer, size_to_copy);

	return size_to_copy - not_copy;
}

static ssize_t proc_write(struct file *FIle, const char __user *buffer, size_t count, loff_t *offset)
{
	return count;
}

static struct file_operations fops = { //DEV
	.open =     dev_open,
	.release =  dev_release,
	.read =     dev_read,
	.write =    dev_write,
	.unlocked_ioctl = dev_ioctl,
};

static struct proc_ops proc_fops = { //PROC
	.proc_read = proc_read,
	.proc_write = proc_write
};

//SYS
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sysfs_emit(buf, "Size of buffer: %d.\nCapasity of buffer: %d.\nBuffer contains:\n%s\n", data_size, BUFFER_SIZE, data_buffer);
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	return count;
}

static struct kobj_attribute chrdev_attr = __ATTR(my_chardev, 0660, sysfs_show, sysfs_store);

//MAIN
static int __init chardev_init(void)
{
	//DEV
	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);

	if (major < 0) {
		pr_err("chardev: register failed: %d.\n", major);
		return major;
	}
	pr_info("chardev: register successfully, major = %d, minor = %d.\n", MAJOR(dev), MINOR(dev));

	cdev_init(&chrdev_cdev, &fops);

	if (cdev_add(&chrdev_cdev, dev, 1) < 0) {
		pr_err("chardev: cdev create failed.\n");
		goto cdev_exit;
	}
	pr_info("chardev: cdev created successfully.\n");

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		pr_err("chardev: device class create failed.\n");
		goto class_exit;
	}
	pr_info("chardev: device class created succsessfully.\n");

	pdev = device_create(pclass, NULL, dev, NULL, CLASS_NAME);
	if (IS_ERR(pdev)) {
		pr_err("chardev: device NODE create failed.\n");
		goto device_exit;
	}
	pr_info("chardev: device node created succsessfully.\n");

	//PROC
	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_folder) {
		pr_err("chardev: create /proc/%s/ folder failed.\n", PROC_DIR_NAME);
		goto device_exit;
	}
	pr_info("chardev: proc folder /proc/%s/ created successfully.\n", PROC_DIR_NAME);

	proc_file = proc_create(PROC_FILE_NAME, 0660, proc_folder, &proc_fops);
	if (!proc_file) {
		pr_err("chardev: initialize /proc/%s/%s failed.\n", PROC_DIR_NAME, PROC_FILE_NAME);
		goto proc_exit;
	}
	pr_info("chardev: /proc/%s/%s initialized successfully.\n", PROC_DIR_NAME, PROC_FILE_NAME);

	//SYS
	chrdev_kobj = kobject_create_and_add(SYSFS_NAME, NULL);

	if (sysfs_create_file(chrdev_kobj, &chrdev_attr.attr)) {
		pr_err("chardev: initialize /sys/%s failed.\n", SYSFS_NAME);
		goto sys_exit;
	}
	pr_info("chardev: /sys/%s initialized successfully.\n", SYSFS_NAME);

	pr_info("chardev: modele init successfully.\n");
	return 0;

sys_exit: //SYS
	sysfs_remove_file(NULL, &chrdev_attr.attr);
	kobject_put(chrdev_kobj);
proc_exit: //PROC
	proc_remove(proc_file);
	proc_remove(proc_folder);
device_exit: //DEV
	class_destroy(pclass);
class_exit:
	cdev_del(&chrdev_cdev);
cdev_exit:
	unregister_chrdev_region(dev, 1);
	return -1;
}

static void __exit chardev_exit(void)
{
	//SYS
	sysfs_remove_file(NULL, &chrdev_attr.attr);
	kobject_put(chrdev_kobj);

	//PROC
	proc_remove(proc_file);
	proc_remove(proc_folder);

	//DEV
	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(dev, 1);

	pr_info("chardev: module exited.\n");
}

module_init(chardev_init);
module_exit(chardev_exit);
