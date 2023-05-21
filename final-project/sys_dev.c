#include <linux/module.h>	
#include <linux/cdev.h>		
#include <linux/device.h>	
#include <linux/kdev_t.h>	
#include <linux/fs.h>		
#include <linux/uaccess.h>	
#include <linux/err.h>		
#include <linux/printk.h>	
#include "sys_dev.h"

ssize_t dev_read (struct file *filep, char *to_user, size_t len, loff_t *offs);
ssize_t dev_write (struct file *filep, const char *from_user, size_t len, loff_t *offs);
int access_to_dev(struct device *dev, struct kobj_uevent_env *env);

dev_t MM;
struct cdev device;
struct class *class_folder;
struct device *dev_file;
struct file_operations dev_fs = {
	.read = dev_read,
	.write = dev_write,
};

MODULE_LICENSE("GPL");

int32_t create_devFS(void)
{
	{//MAJOR AND MINOR
		int32_t err;
		err = alloc_chrdev_region(&MM, 0, 1, CLASS_NAME);
		if(err < 0) {
			pr_err("ultrasound-dev-MM: can not create Major and Minor numbers\n");
			return -ENOMEM;
		}
		pr_info("ultrasound-dev-MM: Major and Minor numbers was created\n");
	}
	{//ADD DEVICE TO SYSTEM WITH FS
		int32_t err;
		cdev_init(&device, &dev_fs);
		err = cdev_add(&device, MM, 1);
		if(err) {
			pr_err("ultrasound-dev-cdev: can not add device to system\n");
			goto cdev_err;
		}
		pr_info("ultrasound-dev-cdev: device was added to system\n");
	}
	{//SYS/CLASS/*
		class_folder = class_create(THIS_MODULE, CLASS_NAME);
		if(IS_ERR(class_folder)) {
			pr_err("ultrasound-dev-class: class on path - /sys/class/%s, can not be created\n", CLASS_NAME);
			goto class_err;
		}
		class_folder->dev_uevent = access_to_dev; //func for access to /dev/*
		pr_info("ultrasound-dev-class: class on path - /sys/class/%s, was created\n", CLASS_NAME);
	}
	{//DEV/*
		dev_file = device_create(class_folder, NULL, MM, NULL, DEVICE_NAME);
		if(IS_ERR(dev_file)) {
			pr_err("ultrasound-dev-device: device file on path - /dev/%s, can not be created\n", DEVICE_NAME);\
			goto dev_err;
		}
		pr_info("ultrasound-dev-device: device file on path - /dev/%s, was created\n", DEVICE_NAME);
	}
	pr_info("ultrasound-dev: devFS was created\n");
	return 0;

dev_err:
	class_destroy(class_folder);
class_err:
	cdev_del(&device);
cdev_err:
	unregister_chrdev_region(MM, 1);
	return -ENOMEM;
}

void remove_devFS(void)
{
	device_destroy(class_folder, MM);
	pr_info("ultrasound-dev-device: device file was removed\n");
	class_destroy(class_folder);
	pr_info("ultrasound-dev-class: class was removed\n");
	cdev_del(&device);
	pr_info("ultrasound-dev-cdev: device was drop from system\n");
	unregister_chrdev_region(MM, 1);
	pr_info("ultrasound-dev-MM: Major and Minor numbers was unregistered\n");
	pr_info("ultrasound-dev: devFS was removed\n");
}

ssize_t dev_read(struct file *filep, char *to_user, size_t len, loff_t *offs)
{
	return 0;
}

ssize_t dev_write(struct file *filep, const char *from_user, size_t len, loff_t *offs)
{
	return len;
}

int access_to_dev(struct device *dev, struct kobj_uevent_env *env)
{
  add_uevent_var(env, "DEVMODE=%#o", 0666);
  return 0;
}