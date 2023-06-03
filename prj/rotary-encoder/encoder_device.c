
#include <linux/init.h>			// Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>		// Core header for loading LKMs into the kernel
#include <linux/kernel.h>		// Contains types, macros, functions for the kernel
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/err.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/ioctl.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include "encoder_ioctl.h"

MODULE_LICENSE("GPL");													///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Yevhen Kolesnyk");										///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Character device driver for rotary encoder");		///< The description -- see modinfo
MODULE_VERSION("0.1");													///< The version of the module

#define ENCODER_GPIO_A 6
#define ENCODER_GPIO_B 5

static struct class *pclass;
static struct device *pdev;
static struct cdev encoder_cdev;
dev_t dev;
static int major;
static int is_open;
static int encoder_num;
static ktime_t last_interrupt_time;
static uint16_t debounce_delay_ms = 10;
static int previousStateA;
static int previousStateB;

static irqreturn_t encoder_irq_handler(int irq, void *dev_id)
{
	int currentStateA, currentStateB;
	ktime_t interrupt_time = ktime_get();

	currentStateA = gpio_get_value(ENCODER_GPIO_A);
	currentStateB = gpio_get_value(ENCODER_GPIO_B);

	if (ktime_to_ms(ktime_sub(interrupt_time, last_interrupt_time)) >= debounce_delay_ms) {
		if (currentStateA != previousStateA || currentStateB != previousStateB) {
			if (previousStateA == 0 && currentStateA == 1 && currentStateB == 0)
				encoder_num++;
			else if (previousStateA == 1 && currentStateA == 0 && currentStateB == 0)
				encoder_num--;
			else if (previousStateA == 0 && currentStateA == 1 && currentStateB == 1)
				encoder_num--;
			else if (previousStateA == 1 && currentStateA == 0 && currentStateB == 1)
				encoder_num++;
			previousStateA = currentStateA;
			previousStateB = currentStateB;
			pr_info("Encoder num: %d\n", encoder_num);
		}
	}
	last_interrupt_time = interrupt_time;

	return IRQ_HANDLED;
}

static ssize_t encoder_num_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("encoder: sysfs show %s", __func__);
	return sprintf(buf, "%d\n", encoder_num);
}

static ssize_t debounce_delay_ms_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("encoder: sysfs show %s", __func__);
	return sprintf(buf, "%d\n", debounce_delay_ms);
}

static ssize_t encoder_num_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int value;

	if (sscanf(buf, "%d", &value) == 1) {
		encoder_num = value;
		return count;
	} else {
		return -EINVAL;
	}
}

static ssize_t debounce_delay_ms_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	int value;

	if (sscanf(buf, "%d", &value) == 1) {
		debounce_delay_ms = value;
		return count;
	} else {
		return -EINVAL;
	}
}

static struct kobject *chrdev_kobj;
struct kobj_attribute encoder_num_attr = __ATTR(chr_encoder_num, 0660, encoder_num_show, encoder_num_store);
struct kobj_attribute debounce_delay_ms_attr = __ATTR(chr_debounce_delay_ms, 0660, debounce_delay_ms_show, debounce_delay_ms_store);

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("encoder: already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("encoder: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("encoder: device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	char encoder_value[16];
	ssize_t num_bytes;

	pr_info("encoder: read from file %s\n", filep->f_path.dentry->d_iname);
	pr_info("encoder: read from device %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));

	snprintf(encoder_value, sizeof(encoder_value), "%d\n", encoder_num);
	num_bytes = strlen(encoder_value);

	if (len < num_bytes) {
		pr_err("encoder: copy to user failed\n");
		return -EINVAL;
	}

	if (copy_to_user(buffer, encoder_value, num_bytes))
		return -EFAULT;

	return num_bytes;
}


static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int err = 0;

	pr_info("encoder:  %s() cmd=0x%x arg=0x%lx\n", __func__, cmd, arg);
	if (_IOC_TYPE(cmd) != CDEV_IOC_MAGIC) {
		pr_err("encoder: CHARDEV_IOC_MAGIC error\n");
		err = -ENOTTY;
	}
	if (_IOC_NR(cmd) >= ENC_IOC_MAXNR) {
		pr_err("encoder:  CHARDEV_IOC_MAXNR err\n");
		err = -ENOTTY;
	}
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (err) {
		pr_err("encoder:  _IOC_READ/_IOC_WRITE err\n");
		err = -EFAULT;
	}

	switch (_IOC_NR(cmd)) {
	case GET_ENCODER:
		if (copy_to_user((int32_t *)arg, &encoder_num, sizeof(encoder_num))) {
			pr_err("encoder:  GET_ENCODER err\n");
			err = -ENOTTY;
		}
		pr_info("encoder:  GET_ENCODER encoder_num=%d\n", encoder_num);
		break;
	case SET_ENCODER:
		if (copy_from_user(&encoder_num, (int32_t *)arg, sizeof(encoder_num))) {
			pr_err("encoder:  SET_ENCODER err\n");
			err = -ENOTTY;
		}
		pr_info("encoder: encoder_num is %d\n", encoder_num);
		break;
	case GET_DEBOUNCE_DELAY:
		if (copy_to_user((int32_t *)arg, &debounce_delay_ms, sizeof(debounce_delay_ms))) {
			pr_err("encoder:  GET_DEBOUNCE_DELAY err\n");
			err = -ENOTTY;
		}
		pr_info("encoder:  GET_DEBOUNCE_DELAY debounce_delay_ms=%d\n", debounce_delay_ms);
		break;
	case SET_DEBOUNCE_DELAY:
		if (copy_from_user(&debounce_delay_ms, (int32_t *)arg, sizeof(debounce_delay_ms))) {
			pr_err("encoder:  SET_DEBOUNCE_DELAY err\n");
			err = -ENOTTY;
		}
		pr_info("encoder: debounce_delay_ms is %d\n", debounce_delay_ms);
		break;
	default:
		pr_warn("encoder:  invalid cmd(%u)\n", cmd);
	}

	return err;
}

static struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.unlocked_ioctl = dev_ioctl,
};

static int __init chrdev_init(void)
{
	int irq;

	encoder_num = 0;
	is_open = 0;
	dev = 0;
	previousStateA = 0;
	previousStateB = 0;
	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);

	if (major < 0) {
		pr_err("encoder: register_chrdev failed: %d\n", major);
		return major;
	}
	pr_info("encoder: register_chrdev ok, major = %d minor = %d\n", MAJOR(dev), MINOR(dev));
	cdev_init(&encoder_cdev, &fops);
	if ((cdev_add(&encoder_cdev, dev, 1)) < 0) {
		pr_err("encoder: cannot add the device to the system\n");
		goto cdev_err;
	}
	pr_info("encoder: cdev created successfully\n");

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass))
		goto class_err;

	pr_info("encoder: device class created successfully\n");
	pdev = device_create(pclass, NULL, dev, NULL, CLASS_NAME"0");
	if (IS_ERR(pdev))
		goto device_err;

	pr_info("encoder: device created successfully\n");
	chrdev_kobj = kobject_create_and_add("encoder_sysfs", kernel_kobj);
	if (sysfs_create_file(chrdev_kobj, &encoder_num_attr.attr)) {
		pr_err("encoder: cannot create sysfs file\n");
		goto encoder_num_err;
	}
	if (sysfs_create_file(chrdev_kobj, &debounce_delay_ms_attr.attr)) {
		pr_err("encoder: cannot create sysfs file\n");
		goto debounce_err;
	}
	if (gpio_request(ENCODER_GPIO_A, "encoder_gpio_a") < 0 ||
		gpio_request(ENCODER_GPIO_B, "encoder_gpio_b") < 0) {
		pr_err("Failed to request GPIOs\n");
		goto gpio_free;
	}
	gpio_direction_input(ENCODER_GPIO_A);
	gpio_direction_input(ENCODER_GPIO_B);
	irq = gpio_to_irq(ENCODER_GPIO_A);
	if (request_irq(irq, encoder_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "encoder_irq", NULL)) {
		pr_err("Failed to request IRQ\n");
		goto gpio_free;
	}

	pr_info("encoder: device driver insmod success\n");
	return 0;
gpio_free:
	gpio_free(ENCODER_GPIO_B);
	gpio_free(ENCODER_GPIO_A);
debounce_err:
	sysfs_remove_file(kernel_kobj, &debounce_delay_ms_attr.attr);
encoder_num_err:
	kobject_put(chrdev_kobj);
device_err:
	device_destroy(pclass, dev);
class_err:
	class_destroy(pclass);
cdev_err:
	unregister_chrdev_region(dev, 1);
	return -EFAULT;
}

static void __exit chrdev_exit(void)
{
	int irq = gpio_to_irq(ENCODER_GPIO_A);

	free_irq(irq, NULL);
	gpio_free(ENCODER_GPIO_B);
	gpio_free(ENCODER_GPIO_A);
	kobject_put(chrdev_kobj);
	sysfs_remove_file(kernel_kobj, &encoder_num_attr.attr);
	sysfs_remove_file(kernel_kobj, &debounce_delay_ms_attr.attr);
	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&encoder_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("encoder: module exited\n");

}

module_init(chrdev_init);
module_exit(chrdev_exit);
