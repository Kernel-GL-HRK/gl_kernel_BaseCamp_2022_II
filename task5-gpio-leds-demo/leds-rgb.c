// SPDX-License-Identifier: GPL-2.0-only
/*
 * Leds GPIO's driver for RGB leds.
 *
 * Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h> // cdev_init
#include <linux/uaccess.h>  //copy_from_user
#include <linux/gpio.h>     //GPIO
#include <linux/export.h>  //export project class name
#include <linux/types.h>
#include "project1.h"

#define MODULE_NAME module_name(THIS_MODULE)
#define DEVICE_NAME "leds_rgb"
#define CLASS_NAME  "project1"

static dev_t dev;
static int is_open;

static struct cdev gpio_leds_cdev;
static struct class *dev_class;
static uint8_t led_num;

unsigned char fs_buffer[BUFFER_MAX_SIZE] = {0};
size_t fs_buffer_size;



/* exporting device class for use with another project modules */
EXPORT_SYMBOL_GPL(dev_class);


static int my_dev_event(struct device *dev, struct kobj_uevent_env *env)
{
	add_uevent_var(env, "DEVMODE=%#o", 0666);
	return 0;
}

static struct file_operations gpio_leds_fops;

 /* Driver init */

static int  __init gpio_leds_driver_main(void)
{

	  /* Allocating chardev */
	if ((alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME)) < 0) {
		pr_err("%s: Cannot allocate major number\n", DEVICE_NAME);
	goto dev_unreg;
	}

	pr_err("%s: Major = %d Minor = %d\n", DEVICE_NAME, MAJOR(dev), MINOR(dev));

	/*Creating cdev structure*/
	cdev_init(&gpio_leds_cdev, &gpio_leds_fops);

	/*Adding chardev to the system*/

	if ((cdev_add(&gpio_leds_cdev, dev, 1)) < 0) {
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

	/* Multiple GPIO's request*/
	if (gpio_request_array(leds_gpios, ARRAY_SIZE(leds_gpios))) {
		pr_err("%s: Cannot request GPIOs\n", DEVICE_NAME);
		goto remove_gpio;
	}
	/* Hiding GPIO's controls from gpio sysfs */
	for (led_num = 0; led_num < ARRAY_SIZE(leds_gpios); led_num++)
		gpio_export(leds_gpios[led_num].gpio, false);

	/* ready UP timers for blinking */
	setup_timers();

	/* Enabling procfs */
	if (leds_rgb_enable_procfs()) {
		pr_err("%s: Cannot create procfs. Exiting\n", DEVICE_NAME);
		goto procfs_fail;
	}
return 0;

procfs_fail:
	leds_control_stop();
	leds_rgb_exit_procfs();
remove_gpio:
	gpio_free_array(leds_gpios, ARRAY_SIZE(leds_gpios));
dev_remove:
	device_destroy(dev_class, dev);
class_del:
	class_destroy(dev_class);
dev_del:
	cdev_del(&gpio_leds_cdev);
dev_unreg:
	unregister_chrdev_region(dev, 1);

return -1;

}

/* Driver exit */

static void  __exit gpio_leds_driver_exit(void)
{
	leds_control_stop();
	leds_rgb_exit_procfs();
	gpio_free_array(leds_gpios, ARRAY_SIZE(leds_gpios));
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&gpio_leds_cdev);
	unregister_chrdev_region(dev, 1);
	pr_err("%s: Quitting driver OK", DEVICE_NAME);

}


/* devsysfs open */
static int device_file_open(struct inode *inode, struct file *file)
{
	if (is_open) {
		pr_err("%s: Can't open. devsysfs is busy", MODULE_NAME);
		return -EBUSY;
	}
	is_open = 1;
	return 0;
}
/* devsysfs release */
static int device_file_release(struct inode *inode, struct file *file)
{
	is_open = 0;
	return 0;
}

/* devsysfs write */
static ssize_t device_file_read(struct file *filp, char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t ret;

	if (*offset >= fs_buffer_size) {
		/* we have finished to read, return 0 */
		ret  = 0;
	} else {
		/* fill the buffer, return the buffer size */
		copy_to_user(buffer, fs_buffer, fs_buffer_size);
		*offset += fs_buffer_size;
		ret = fs_buffer_size;
	}
	return ret;
}
/* devsysfs read */
static ssize_t device_file_write(struct file *filp, const char __user *buffer, size_t count, loff_t *offset)
{

	fs_buffer_size = count; //store buffer size
	memset(fs_buffer, 0, BUFFER_MAX_SIZE); //clear buffer from old garbage

	/* check for write buffer overflow > 1024 */
	if (fs_buffer_size > BUFFER_MAX_SIZE)
		fs_buffer_size = BUFFER_MAX_SIZE;

	if (copy_from_user(fs_buffer, buffer, fs_buffer_size)) {
		pr_err("%s: procfs write failed\n", MODULE_NAME);
		return -EFAULT;
	}
	pr_err("%s: Driver write = %s\n", MODULE_NAME, fs_buffer);
	if (leds_control(fs_buffer))
		pr_err("%s: write blink delay failed\n", MODULE_NAME);

	return count;
}


static struct file_operations gpio_leds_fops = {
	.owner  = THIS_MODULE,
	.read	= device_file_read,
	.write  = device_file_write,
	.open   = device_file_open,
	.release  = device_file_release,
};


module_init(gpio_leds_driver_main);
module_exit(gpio_leds_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eddie07 <splissken2014@gmail.com>");
MODULE_DESCRIPTION("Custom GPIO's driver for RGB leds");
MODULE_VERSION("0.1");
