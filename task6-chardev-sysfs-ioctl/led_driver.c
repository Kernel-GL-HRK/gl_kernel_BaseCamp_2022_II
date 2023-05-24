// SPDX-License-Identifier: GPL
/**
 * Simple GPIO LED Character Device Driver
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/timer.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>

#include "led_driver.h"


#define GPIO_PIN	26
#define DEVICE_DESC	"gpio26"

#define BUFFER_SIZE	512
#define DEFAULT_DELAY	500

#define MIN_DELAY_VALUE	100
#define MAX_DELAY_VALUE	30000


/* chardev part */

dev_t dev;
static struct class *dev_class;
static struct cdev led_cdev;

static int32_t delay = DEFAULT_DELAY;
static struct kobject *kobj;

static struct timer_list timer;

static int __init led_driver_init(void);
static void __exit led_driver_exit(void);

static int led_open(struct inode *inode, struct file *f);
static int led_release(struct inode *inode, struct file *f);
static ssize_t led_read(struct file *f,
			char __user *buf, size_t len, loff_t *off);
static ssize_t led_write(struct file *f,
			 const char *buf, size_t len, loff_t *off);
static long led_ioctl(struct file *f, unsigned int cmd, unsigned long arg);

static const struct file_operations fops = {
	.owner		= THIS_MODULE,
	.read		= led_read,
	.write		= led_write,
	.open		= led_open,
	.release	= led_release,
	.unlocked_ioctl = led_ioctl,
};


/* chrdrv part */

static int led_open(struct inode *inode, struct file *f)
{
	/* Device File Opened */
	return 0;
}

static int led_release(struct inode *inode, struct file *f)
{
	/* Device File Closed */
	return 0;
}

static ssize_t led_read(struct file *f,
			char __user *buf, size_t len, loff_t *off)
{
	uint8_t gpio_state = 0;

	/* Reading GPIO value */
	gpio_state = gpio_get_value(GPIO_PIN);

	/* Write to user */
	len = 1;
	if (copy_to_user(buf, &gpio_state, len) > 0)
		pr_err("ERROR: Not all the bytes have been copied to user\n");

	pr_info("Read function: GPIO_%d = %d\n", GPIO_PIN, gpio_state);

	return 0;
}

static ssize_t led_write(struct file *f,
			 const char __user *buf, size_t len, loff_t *off)
{
	uint8_t rec_buf[BUFFER_SIZE] = {0};

	if (copy_from_user(rec_buf, buf, len) > 0)
		pr_err("ERROR: Not all the bytes have been copied from user\n");

	pr_info("Write Function: GPIO_%d = %c\n", GPIO_PIN, rec_buf[0]);

	if (rec_buf[0] == '1') {
		/* set the GPIO value to HIGH */
		gpio_set_value(GPIO_PIN, 1);
	} else if (rec_buf[0] == '0') {
		/* set the GPIO value to LOW */
		gpio_set_value(GPIO_PIN, 0);
	} else {
		pr_err("Unknown command: Please provide either 1 or 0\n");
	}

	return len;
}


/* ioctl part */

static long led_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	int32_t value;

	pr_info("%s()  cmd = 0x%x  arg = 0x%lx\n", __func__, cmd, arg);

	if (_IOC_TYPE(cmd) != IOCTL_MAGIC) {
		pr_err("Wrong magic number!\n");
		return -ENOTTY;
	}

	if (_IOC_NR(cmd) >= IOCTL_MAXN) {
		pr_err("Wrong command!\n");
		return -ENOTTY;
	}

	switch (cmd) {
	case LED_WR_DELAY:
		if (copy_from_user(&value, (int32_t *) arg, sizeof(value)))
			pr_err("Data write error!");

		if (value >= MIN_DELAY_VALUE && value <= MAX_DELAY_VALUE) {
			delay = value;
			pr_info("Delay changed to %d\n", delay);
		}
		break;

	case LED_RD_DELAY:
		if (copy_to_user((int32_t *) arg, &delay, sizeof(delay)))
			pr_err("Data read error!");
		pr_info("Delay is %d\n", delay);
		break;

	default:
		return -ENOTTY;
	}

	return 0;
}


/* sysfs part */

static ssize_t delay_show(struct kobject *kobj,
			  struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", delay);
}

static ssize_t delay_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t count)
{
	if (kstrtos32(buf, 0, &delay) < 0)
		delay = DEFAULT_DELAY;

	return count;
}

static struct kobj_attribute delay_attribute =
	__ATTR(delay, 0664, delay_show, delay_store);



/* timer */

static void timer_callback(struct timer_list *data)
{
	static unsigned int led_state;

	led_state ^= 1;

	gpio_set_value(GPIO_PIN, led_state);
	mod_timer(&timer, jiffies + msecs_to_jiffies(delay));
}


/* init, exit part */

static int __init led_driver_init(void)
{
	/* Allocating Major number */
	if ((alloc_chrdev_region(&dev, 0, 1, DEV_NAME)) < 0) {
		pr_err("Cannot allocate major number\n");
		goto r_unreg;
	}
	pr_info("Major = %d, Minor = %d\n", MAJOR(dev), MINOR(dev));

	/* Creating cdev structure */
	cdev_init(&led_cdev, &fops);

	/* Adding character device to the system */
	if ((cdev_add(&led_cdev, dev, 1)) < 0) {
		pr_err("Cannot add the device to the system\n");
		goto r_del;
	}

	/* Creating struct class */
	dev_class = class_create(THIS_MODULE, DEV_CLASS);
	if (IS_ERR(dev_class)) {
		pr_err("Cannot create the struct class\n");
		goto r_class;
	}

	/* Creating device */
	if (IS_ERR(device_create(dev_class, NULL, dev, NULL, DEV_NAME))) {
		pr_err("Cannot create device\n");
		goto r_device;
	}

	/* Checking the GPIO is valid or not */
	if (gpio_is_valid(GPIO_PIN) == false) {
		pr_err("ERROR: GPIO %d is not valid\n", GPIO_PIN);
		goto r_device;
	}

	/* Requesting the GPIO */
	if (gpio_request(GPIO_PIN, DEVICE_DESC) < 0) {
		pr_err("ERROR: GPIO %d request\n", GPIO_PIN);
		goto r_gpio;
	}

	gpio_direction_output(GPIO_PIN, 0);
	gpio_export(GPIO_PIN, false);


	/* Creating sysfs kobj */
	kobj = kobject_create_and_add(DEV_NAME, NULL);
	if (!kobj) {
		pr_err("ERROR: Cannot create kobject\n");
		goto r_gpio;
	}

	/* Creating sysfs delay attribute file */
	if (sysfs_create_file(kobj, &delay_attribute.attr)) {
		pr_err("ERROR: Failed to create sysfs file\n");
		goto r_sysfs;
	}

	timer_setup(&timer, timer_callback, 0);
	add_timer(&timer);

	pr_info("LED Driver Inserted.\n");
	return 0;

r_sysfs:
	kobject_put(kobj);
r_gpio:
	gpio_free(GPIO_PIN);
r_device:
	device_destroy(dev_class, dev);
r_class:
	class_destroy(dev_class);
r_del:
	cdev_del(&led_cdev);
r_unreg:
	unregister_chrdev_region(dev, 1);

	return -EPERM;
}

static void __exit led_driver_exit(void)
{
	gpio_set_value(GPIO_PIN, 0);
	del_timer(&timer);

	sysfs_remove_file(kobj, &delay_attribute.attr);
	kobject_put(kobj);

	gpio_unexport(GPIO_PIN);
	gpio_free(GPIO_PIN);

	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&led_cdev);
	unregister_chrdev_region(dev, 1);

	pr_info("LED Driver Removed.\n");
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vlad Degtyarov <deesyync@gmail.com>");
MODULE_DESCRIPTION("A simple GPIO LED chardev driver");
MODULE_VERSION("0.1");
