// SPDX-License-Identifier: GPL-2.0-only
/*
 * Module library: sc
 * Description: Device driver for the module
 * Module: smart-clock
 *
 * Copyright (C) 2023 Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h> // cdev_init
#include <linux/uaccess.h>  //copy_from_user
#include <linux/export.h>  //export project class name
#include <linux/slab.h>
#include "include/project1.h"


#define DEVICE_NAME "smart_clock"
#define CLASS_NAME  "project1"


static dev_t dev;
static struct cdev smart_clock_cdev;
static struct class *dev_class;
static int is_open;
static struct file_operations smart_clock_fops;

/**
 * Init external structures and variables used in module libraries
 */
struct options options;
struct clock_timer our_timer;
struct init_hw  init_hw;
struct fs_buffer fs_buffer = { .buf = NULL };
struct clock_and_alarm clock_and_alarm;
struct temp_and_press temp_and_press;
struct game game;
struct button my_button;

/* exporting device class for use with another project modules */
EXPORT_SYMBOL_GPL(dev_class);

/**
 * struct my_dev_event()- uevent to add our device sysfs readable and writabe without root permissions.
 */
static int my_dev_event(struct device *dev, struct kobj_uevent_env *env)
{
	add_uevent_var(env, "DEVMODE=%#o", 0666);
	return 0;
}

/**
 * smart_clock_main() - main module init.
 *
 * Creating device, class and sysfs, stating module libraries
 */
static int  __init smart_clock_main(void)
{

	 /* Allocating chardev */
	if ((alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME)) < 0) {
		pr_err("%s: Cannot allocate major number\n", DEVICE_NAME);
	goto dev_unreg;
	}

	pr_err("%s: Major = %d Minor = %d\n", DEVICE_NAME, MAJOR(dev), MINOR(dev));

	/*Creating cdev structure*/
	cdev_init(&smart_clock_cdev, &smart_clock_fops);

	/*Adding chardev to the system*/

	if ((cdev_add(&smart_clock_cdev, dev, 1)) < 0) {
		pr_err("%s: Cannot add the device to the system\n", DEVICE_NAME);
	goto dev_del;
	}

	/*Creating struct class*/

	dev_class = class_create(THIS_MODULE, CLASS_NAME);
	if (dev_class == NULL) {
		pr_err("%s: Cannot create the struct class\n", DEVICE_NAME);
		goto class_del;
	}
	/* Device node access permissions*/
	dev_class->dev_uevent = my_dev_event;

	/*Creating device*/
	if (device_create(dev_class, NULL, dev, NULL, DEVICE_NAME) == NULL) {
		pr_err("%s: Cannot create the Device\n", DEVICE_NAME);
		goto dev_remove;
	}

	/*Init GPIO*/
	if (gpio_button_init()) {
		pr_err("GPIO button init fail\n");
		goto dev_remove;
		}
	/*Init display*/
	st7735fb_init();
	if (!init_hw.st7735_spi_probed) {
		pr_err("st7735 spi panel init fail\n");
		goto hw_panel_fail;
		}
	/*Init sensors and rtc*/
	sensors_init();
	if (!init_hw.bmp280_i2c_probed) {
		pr_err("bmp280 i2c sensor init fail\n");
		goto hw_sensors_fail;
		}
	if (!init_hw.ds3231_i2c_probed) {
		pr_err("ds3231 i2c sensor init fail\n");
		goto hw_sensors_fail;
		}
	if (!init_hw.mpu6050_i2c_probed) {
		pr_err("mpu6050 i2c sensor init fail\n");
		goto hw_sensors_fail;
		}
	/*Init controls*/
	init_controls();


return 0;


hw_sensors_fail:
	sensors_unload();
hw_panel_fail:
	st7735fb_unload();
	gpio_button_unload();
dev_remove:
	device_destroy(dev_class, dev);
class_del:
	class_destroy(dev_class);
dev_del:
	cdev_del(&smart_clock_cdev);
dev_unreg:
	unregister_chrdev_region(dev, 1);

return -1;
}

/**
 * smart_clock_exit() - run at module exit.
 *
 * unload libraries
 */
static void  __exit smart_clock_exit(void)
{
	controls_unload();
	sensors_unload();
	gpio_button_unload();
	st7735fb_unload();
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&smart_clock_cdev);
	unregister_chrdev_region(dev, 1);
	pr_err("module unloaded\n");
}


/**
 * device_file_open() - devsysfs open for r/w.
 *
 * we check @is_open value if devfs is already in access by another process
 */
static int device_file_open(struct inode *inode, struct file *file)
{
	if (is_open) {
		pr_err("%s: Can't open. devsysfs is busy", DEVICE_NAME);
		return -EBUSY;
	}
	is_open = 1;
	return 0;
}

/**
 * device_file_release() - devsysfs release.
 *
 * we reset @is_open if devfs is released
 */
static int device_file_release(struct inode *inode, struct file *file)
{
	is_open = 0;
	return 0;
}


/**
 * device_file_read() - read sysfs.
 *
 * Used to dump screen to bmp format in userspace.
 */
static ssize_t device_file_read(struct file *filp, char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t ret;
	fs_buffer.buf=NULL;

	if (!*offset)
		if (st7735fb_get_buff_display() < 0)
			ret = 0;

	if ((*offset >= fs_buffer.buf_len)  ) {
		/* we have finished to read, return 0 */
		ret = 0;
	} else {
		/* fill the buffer, return the buffer size */
		copy_to_user(buffer, fs_buffer.buf, fs_buffer.buf_len);
		*offset += fs_buffer.buf_len;
		ret = fs_buffer.buf_len;
	}
	if (fs_buffer.buf)
		pr_err ("kfree");
		kfree(fs_buffer.buf);	
	return ret;
}

/**
 * device_file_write() - write to sysfs.
 *
 * Write file from userspace. Used to read bmp format images from userspace
 */
static ssize_t device_file_write(struct file *filp, const char __user *buffer, size_t count, loff_t *offset)
{

	fs_buffer.buf_len = count;

	fs_buffer.buf = kzalloc(count + 1, GFP_KERNEL);
	if (fs_buffer.buf == NULL) {
		pr_err("%s: malloc for buffer failed", __func__);
		return -ENOMEM;
	}
	if (copy_from_user(fs_buffer.buf, buffer, fs_buffer.buf_len)) {
		pr_err("%s: sysfs write failed\n", __func__);
		return -EFAULT;
	}

	/* send buffer to vmem and update the screen, if fail- return 0 */
	if (st7735fb_send_buff_display())
		count = 0;
	else
		st7735fb_update_display();
	/* Release buffer and cleanup */
	kfree(fs_buffer.buf);
	fs_buffer.buf_len = 0;
	return count;
}

/**
 * struct smart_clock_fops -fops for sysfs.
 */
static struct file_operations smart_clock_fops = {
	.owner  = THIS_MODULE,
	.read	= device_file_read,
	.write  = device_file_write,
	.open   = device_file_open,
	.release  = device_file_release,
};



module_init(smart_clock_main);
module_exit(smart_clock_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytro Volkov <splissken2014@gmail.com>");
MODULE_DESCRIPTION("smart-clock device");
MODULE_VERSION("0.3");
