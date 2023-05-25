#include <linux/init.h>		 // Macros used to mark up functions e.g., __init __exit
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
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include "cdev_ioctl.h"
#include "st7735.h"

MODULE_LICENSE("GPL");				  ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Yevhen Kolesnyk");		///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Character device driver for st7735 with Proc, Sys Interface and IOCTL");  ///< The description -- see modinfo
MODULE_VERSION("0.1");				  ///< The version of the module

#define PROC_FILE_NAME "dev_st7735"
#define PROC_DIR_NAME "chr_st7735"
#define MAX_BUFFER_SIZE 1024
#define MAX_COLOR_NAME_SIZE 10
#define MAX_FONT_SIZE 5

static struct spi_device *spi;
static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;
static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev;
dev_t dev;
static int major;
static int is_open;
static size_t buffer_size;
static unsigned char data_buffer[MAX_BUFFER_SIZE] = {0};
static unsigned char color_name[MAX_COLOR_NAME_SIZE] = {0};
struct display_font dev_font;

static ssize_t data_buffer_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("st7735: sysfs show %s", __func__);
	return sprintf(buf, "%s\n", data_buffer);
}

static ssize_t text_color_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("st7735: sysfs show %s", __func__);
	return sprintf(buf, "%s\n", color_show(color_name, dev_font.text_color));
}

static ssize_t font_size_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("st7735: sysfs show %s", __func__);
	return sprintf(buf, "%d\n", dev_font.font_size);
}

static ssize_t bg_color_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("st7735: sysfs show %s", __func__);
	return sprintf(buf, "%s\n", color_show(color_name, dev_font.bg_color));
}

static struct kobject *chrdev_kobj;
struct kobj_attribute data_buffer_attr = __ATTR(chrdev_buffer, 0660, data_buffer_show, NULL);
struct kobj_attribute data_text_color_attr = __ATTR(chrdev_text_color, 0660, text_color_show, NULL);
struct kobj_attribute font_size_attr = __ATTR(chrdev_font_size, 0660, font_size_show, NULL);
struct kobj_attribute bg_color_attr = __ATTR(chrdev_bg_color, 0660, bg_color_show, NULL);

static ssize_t proc_fs_read(struct file *File, char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (*offset >= buffer_size)
		return 0;
	to_copy = min(count, buffer_size - (size_t)*offset);
	not_copied = copy_to_user(buffer, data_buffer + *offset, to_copy);
	delta = to_copy - not_copied;
	*offset += delta;

	return delta;
}

static struct proc_ops proc_fops = {
	.proc_read = proc_fs_read,
};

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("st7735: already open\n");
		return -EBUSY;
	}
	is_open = 1;
	pr_info("st7735: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("st7735: device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int ret;

	pr_info("st7735: read from file %s\n", filep->f_path.dentry->d_iname);
	pr_info("st7735: read from device %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));
	if (len > buffer_size)
		len = buffer_size;
	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
		pr_err("st7735: copy to user failed: %d\n", ret);
		return -EFAULT;
	}
	buffer_size = 0;
	pr_info("st7735: %zu bytes read\n", len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int ret;

	pr_info("st7735: write to file %s\n", filep->f_path.dentry->d_iname);
	pr_info("st7735: write to device %d: %d\n", imajor(filep->f_inode), iminor(filep->f_inode));
	buffer_size = len;
	if (buffer_size > MAX_BUFFER_SIZE)
		buffer_size = MAX_BUFFER_SIZE;
	ret = copy_from_user(data_buffer, buffer, buffer_size);
	if (ret) {
		pr_err("st7735: copy_from_user failed: %d\n", ret);
		return -EFAULT;
	}
	pr_info("st7735: %d bytes written\n", buffer_size);
	if (ST7735_DrawString(data_buffer, buffer_size, &dev_font, spi))
		pr_err("st7735: ST7735_DrawString fail");

	return buffer_size;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int16_t value = 0;

	pr_info("st7735:  %s() cmd=0x%x arg=0x%lx\n", __func__, cmd, arg);
	if (_IOC_TYPE(cmd) != CDEV_IOC_MAGIC) {
		pr_err("st7735: CHARDEV_IOC_MAGIC error\n");
		err = -ENOTTY;
	}
	if (_IOC_NR(cmd) >= CDEV_IOC_MAXNR) {
		pr_err("st7735:  CHARDEV_IOC_MAXNR err\n");
		err = -ENOTTY;
	}
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (err) {
		pr_err("st7735:  _IOC_READ/_IOC_WRITE err\n");
		err = -EFAULT;
	}
	switch (_IOC_NR(cmd)) {
	case CLEAR_BUFFER:
		buffer_size = 0;
		data_buffer[0] = '\0';
		clear_display(dev_font.bg_color, spi);
		pr_info("st7735: CLEAR_BUFFER\n");
		break;
	case GET_FONT_SIZE:
		if (copy_to_user((int16_t *)arg, &dev_font.font_size, sizeof(dev_font.font_size))) {
			pr_err("st7735: GET_FONT_SIZE err\n");
			err = -ENOTTY;
		}
		pr_info("st7735: GET_FONT_SIZE font_size=%d\n", dev_font.font_size);
		break;
	case SET_FONT_SIZE:
		if (copy_from_user(&dev_font.font_size, (int16_t *)arg, sizeof(dev_font.font_size))) {
			pr_err("st7735: SET_FONT_SIZE err\n");
			err = -ENOTTY;
		}
		if (dev_font.font_size > MAX_FONT_SIZE)
			dev_font.font_size = MAX_FONT_SIZE;
		pr_info("st7735: font_size is %d\n", dev_font.font_size);
		break;
	case GET_TEXT_COLOR:
		if (copy_to_user((int16_t *)arg, &dev_font.text_color, sizeof(dev_font.text_color))) {
			pr_err("st7735:  GET_TEXT_COLOR err\n");
			err = -ENOTTY;
		}
		pr_info("st7735: GET_TEXT_COLOR current color is %s\n", color_show(color_name, dev_font.text_color));
		break;
	case SET_TEXT_COLOR:
		if (copy_from_user(&dev_font.text_color, (int16_t *)arg, sizeof(dev_font.text_color))) {
			pr_err("st7735: SET_TEXT_COLOR err\n");
			err = -ENOTTY;
		}
		if (color_show(color_name, dev_font.text_color)[0] == 'U')
			dev_font.text_color = WHITE;
		pr_info("st7735: SET_TEXT_COLOR current color is %s\n", color_show(color_name, dev_font.text_color));
		break;
	case GET_BG_COLOR:
		if (copy_to_user((int16_t *)arg, &dev_font.bg_color, sizeof(dev_font.bg_color))) {
			pr_err("st7735:  GET_BG_COLOR err\n");
			err = -ENOTTY;
		}
		pr_info("st7735: GET_BG_COLOR current color is %s\n", color_show(color_name, dev_font.bg_color));
		break;
	case SET_BG_COLOR:
		if (copy_from_user(&dev_font.bg_color, (int16_t *)arg, sizeof(dev_font.bg_color))) {
			pr_err("st7735: SET_BG_COLOR err\n");
			err = -ENOTTY;
		}
		if (color_show(color_name, dev_font.bg_color)[0] == 'U')
			dev_font.bg_color = BLACK;
		pr_info("st7735: SET_BG_COLOR current color is %s\n", color_show(color_name, dev_font.bg_color));
		break;
	case SET_X:
		if (copy_from_user(&value, (int16_t *)arg, sizeof(value))) {
			pr_err("st7735: SET_BG_COLOR err\n");
			err = -ENOTTY;
		}
		dev_font.x = value;
		pr_info("st7735: SET_X current color is %d\n", dev_font.x);
		break;
	case SET_Y:
		if (copy_from_user(&value, (int16_t *)arg, sizeof(value))) {
			pr_err("st7735: SET_BG_COLOR err\n");
			err = -ENOTTY;
		}
		dev_font.y = value;
		pr_info("st7735: SET_Y current color is %d\n", dev_font.y);
		break;
	default:
		pr_warn("st7735:  invalid cmd(%u)\n", cmd);
	}

	return err;
}

static struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
	.unlocked_ioctl = dev_ioctl,
};

static int spi_init(void)
{
	int ret;

	struct spi_master *master;
	struct spi_board_info spi_device_info = {
		.modalias = "my_st7735",
		.max_speed_hz = 62000000,
		.bus_num = 1,
		.chip_select = 0,
		.mode = SPI_MODE_0,
	};
	pr_info("st7735: spi basic driver init");

	master = spi_busnum_to_master(spi_device_info.bus_num);
	if (!master) {
		pr_err("st7735: Failed to create master device");
		return -ENODEV;
	}
	spi = spi_new_device(master, &spi_device_info);
	if (!spi) {
		pr_err("st7735: Failed to create slave device");
		return -ENODEV;
	}
	spi->bits_per_word = 8;
	ret = spi_setup(spi);
	if (ret) {
		pr_err("st7735: Failed to setup slave");
		spi_unregister_device(spi);
		return -ENODEV;
	}
	mdelay(100);
	pr_info("st7735: init dispaly");
	_init_display(spi);
	clear_display(dev_font.bg_color, spi);

	return 0;
}


static int __init chrdev_init(void)
{

	is_open = 0;
	buffer_size = 0;
	dev = 0;
	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
	dev_font.font_size = 3;
	dev_font.text_color = WHITE;
	dev_font.bg_color = BLACK;
	dev_font.x = 1;
	dev_font.y = 1;

	if (major < 0) {
		pr_err("st7735: register_chrdev failed: %d\n", major);
		return major;
	}
	pr_info("st7735: register_chrdev ok, major = %d minor = %d\n", MAJOR(dev), MINOR(dev));
	cdev_init(&chrdev_cdev, &fops);
	if ((cdev_add(&chrdev_cdev, dev, 1)) < 0) {
		pr_err("st7735: cannot add the device to the system\n");
		goto cdev_err;
	}
	pr_info("st7735: cdev created successfully\n");
	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass))
		goto class_err;
	pr_info("st7735: device class created successfully\n");
	pdev = device_create(pclass, NULL, dev, NULL, CLASS_NAME"0");
	if (IS_ERR(pdev))
		goto device_err;
	pr_info("st7735: device created successfully\n");
	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if (proc_folder == NULL) {
		pr_err("st7735: error creating proc directory\n");
		goto proc_dir_err;
	}
	pr_info("st7735: proc directory created successfully\n");
	proc_file = proc_create(PROC_FILE_NAME, 0644, proc_folder, &proc_fops);
	if (proc_file == NULL) {
		pr_err("st7735: error creating proc file\n");
		goto proc_file_err;
	}
	pr_info("st7735: proc file created successfully\n");
	chrdev_kobj = kobject_create_and_add("st7735_sysfs", kernel_kobj);
	if (sysfs_create_file(chrdev_kobj, &data_buffer_attr.attr)) {
		pr_err("st7735: cannot create sysfs file\n");
		goto sysfs_buffer_err;
	}
	if (sysfs_create_file(chrdev_kobj, &data_text_color_attr.attr)) {
		pr_err("st7735: cannot create sysfs file\n");
		goto sysfs_text_color_err;
	}
	if (sysfs_create_file(chrdev_kobj, &font_size_attr.attr)) {
		pr_err("st7735: cannot create sysfs file\n");
		goto sysfs_read_count_err;
	}
	if (sysfs_create_file(chrdev_kobj, &bg_color_attr.attr)) {
		pr_err("st7735: cannot create sysfs file\n");
		goto sysfs_write_count_err;
	}
	pr_info("st7735: device driver insmod success\n");
	if (spi_init()) {
		pr_info("st7735: spi_init(spi) failed");
		goto spi_init_err;
	}
	if (gpio_request(RST, "RST") && gpio_request(DC, "DC"))
		pr_info("st7735: failed in gpio request");

	gpio_direction_output(RST, 1);
	gpio_direction_output(DC, 1);

	return 0;

spi_init_err:
	spi_unregister_device(spi);
sysfs_write_count_err:
	sysfs_remove_file(kernel_kobj, &font_size_attr.attr);
sysfs_read_count_err:
	sysfs_remove_file(kernel_kobj, &data_text_color_attr.attr);
sysfs_text_color_err:
	sysfs_remove_file(kernel_kobj, &data_buffer_attr.attr);
sysfs_buffer_err:
	kobject_put(chrdev_kobj);
proc_file_err:
	remove_proc_entry(PROC_FILE_NAME, proc_folder);
proc_dir_err:
	remove_proc_entry(PROC_DIR_NAME, NULL);
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
	gpio_free(RST);
	gpio_free(DC);
	spi_unregister_device(spi);
	kobject_put(chrdev_kobj);
	sysfs_remove_file(kernel_kobj, &font_size_attr.attr);
	sysfs_remove_file(kernel_kobj, &data_text_color_attr.attr);
	sysfs_remove_file(kernel_kobj, &data_buffer_attr.attr);
	sysfs_remove_file(kernel_kobj, &bg_color_attr.attr);
	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("st7735: module exited\n");
	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("st7735: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
}

module_init(chrdev_init);
module_exit(chrdev_exit);
