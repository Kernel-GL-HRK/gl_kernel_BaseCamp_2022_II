#include <linux/module.h>		//for macros of module
#include <linux/cdev.h>			//for functions and structures cdev
#include <linux/device.h>		//for functions and structures device
#include <linux/kdev_t.h>		//for macroses and types major and minor numbers
#include <linux/fs.h>			//for calls read() and write()
#include <linux/uaccess.h>		//for copy_to(from)_user
#include <linux/err.h>			//for definions of system errors
#include <linux/printk.h>		//for printk
#include <linux/string.h>		//for string operations
#include "sys_dev.h"
#include "servo_ctl.h"			//for currentAngle variable
#include "platform_driver.h"	//for servo_params

MODULE_LICENSE("GPL");

static dev_t MM;
static struct cdev device;
static struct class *class_folder;
static struct device *dev_file;

static ssize_t dev_read(struct file *filep, char *to_user, size_t len, loff_t *offs)
{
	int8_t buffer_for_copy[MAX_BUFFER_SIZE] = {0};
	size_t to_copy = 0;
	struct servo_params status_servo;

	status_servo = get_servo_description();
	if (!strcmp(status_servo.status, "enabled")) {
		to_copy = snprintf(buffer_for_copy, MAX_BUFFER_SIZE, "\rCurrent angle of servo = [%d]        ", currentAngle);
		to_copy = min(to_copy, len);
	} else {
		to_copy = snprintf(buffer_for_copy, MAX_BUFFER_SIZE, "\rCurrent angle of servo = [undefined]");
		to_copy = min(to_copy, len);
	}

	if (copy_to_user(to_user, buffer_for_copy, to_copy)) {
		pr_err("servo-dev-read: can not send buffer to user\n");
		return -ENOMEM;
	}

	return to_copy;
}

static ssize_t dev_write(struct file *filep, const char *from_user, size_t len, loff_t *offs)
{
	int8_t buffer_for_copy[MAX_BUFFER_SIZE] = {0};
	int32_t converted = 0, polarity = 0;
	uint32_t i, not_converted = 0;
	struct servo_params status_servo;

	if (copy_from_user(buffer_for_copy, from_user, len)) {
		pr_err("servo-dev-write: can not send buffer to kernel\n");
		return -ENOMEM;
	}

	for (i = 0; i < len; i++) {
		if ((buffer_for_copy[i] >= '0') && (buffer_for_copy[i] <= '9'))
			converted = (converted * 10) + (buffer_for_copy[i] - '0');
		else if ((buffer_for_copy[i] == '-') && (converted == 0)) {
			polarity = 1;
			not_converted++;
		} else {
			not_converted++;\
		}
	}

	if (not_converted == len) {
		pr_warn("servo-dev-write: buffer does not have ane digits, servo will not move\n");
		return len;
	}

	status_servo = get_servo_description();
	if (!strcmp(status_servo.status, "enabled")) {
		if (!strcmp(status_servo.mode, "absolute")) {
			servo_set_angle_abs(converted);
		} else if (!strcmp(status_servo.mode, "relative")) {
			if (polarity)
				converted *= -1;
			servo_set_angle_rel (converted);
		} else {
			pr_warn("servo-dev-write: servo mode in device tree is wrong, servo will not move\n");
		}
	} else {
		pr_warn("servo-dev-write: servo is disabled, servo will not move\n");
	}
	return len;
}

loff_t dev_seek(struct file *filp, loff_t off, int whence)
{
    loff_t new_pos;

    switch(whence) {
      case SEEK_SET:
        new_pos = off;
        break;

      case SEEK_CUR:
        new_pos = filp->f_pos + off;
        break;

      default: // can't happen
        return -EINVAL;
    }
    if (new_pos < 0) return -EINVAL;
    filp->f_pos = new_pos;
    return new_pos;
}

static struct file_operations dev_fs = {
	.read = dev_read,
	.write = dev_write,
	.llseek = dev_seek,
};

static int access_to_dev(struct device *dev, struct kobj_uevent_env *env)
{
	add_uevent_var(env, "DEVMODE=%#o", 0666);
	return 0;
}

int32_t create_devFS(void)
{
	{//MAJOR AND MINOR
		int32_t err;

		err = alloc_chrdev_region(&MM, 0, 1, CLASS_NAME);
		if (err < 0) {
			pr_err("servo-dev-MM: can not create Major and Minor numbers\n");
			return -ENOMEM;
		}
		pr_info("servo-dev-MM: Major and Minor numbers was created\n");
	}
	{//ADD DEVICE TO SYSTEM WITH FS
		int32_t err;

		cdev_init(&device, &dev_fs);
		err = cdev_add(&device, MM, 1);
		if (err) {
			pr_err("servo-dev-cdev: can not add device to system\n");
			goto cdev_err;
		}
		pr_info("servo-dev-cdev: device was added to system\n");
	}
	{//SYS/CLASS/*
		class_folder = class_create(THIS_MODULE, CLASS_NAME);
		if (IS_ERR(class_folder)) {
			pr_err("servo-dev-class: class on path - /sys/class/%s, can not be created\n", CLASS_NAME);
			goto class_err;
		}
		class_folder->dev_uevent = access_to_dev; //func for access to /dev/*
		pr_info("servo-dev-class: class on path - /sys/class/%s, was created\n", CLASS_NAME);
	}
	{//DEV/*
		dev_file = device_create(class_folder, NULL, MM, NULL, DEVICE_NAME);
		if (IS_ERR(dev_file)) {
			pr_err("servo-dev-device: device file on path - /dev/%s, can not be created\n", DEVICE_NAME);\
			goto dev_err;
		}
		pr_info("servo-dev-device: device file on path - /dev/%s, was created\n", DEVICE_NAME);
	}
	pr_info("servo-dev: devFS was created\n");
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
	pr_info("servo-dev-device: device file was removed\n");
	class_destroy(class_folder);
	pr_info("servo-dev-class: class was removed\n");
	cdev_del(&device);
	pr_info("servo-dev-cdev: device was drop from system\n");
	unregister_chrdev_region(MM, 1);
	pr_info("servo-dev-MM: Major and Minor numbers was unregistered\n");
	pr_info("servo-dev: devFS was removed\n");
}