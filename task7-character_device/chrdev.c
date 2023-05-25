#include <linux/init.h>         // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>       // Core header for loading LKMs into the kernel
#include <linux/kernel.h>       // Contains types, macros, functions for the kernel
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

#include "cdev_ioctl.h"

MODULE_LICENSE("GPL");                  ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Yevhen Kolesnyk");       ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Character device driver with Proc, Sys Interface and IOCTL");  ///< The description -- see modinfo
MODULE_VERSION("0.1");                  ///< The version of the module

#define MAX_BUFFER_SIZE 1024

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

#define PROC_FILE_NAME "chrdev_info"
#define PROC_DIR_NAME "chrdev"

static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev;
dev_t dev;

static int major;
static int is_open;

static size_t buffer_size;
static unsigned char data_buffer[MAX_BUFFER_SIZE] = {0};


static int dev_read_count;
static int dev_write_count;


static ssize_t data_buffer_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("chrdev: sysfs show %s", __func__);
	return sprintf(buf, "%s\n", data_buffer);
}

static ssize_t buffer_size_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("chrdev: sysfs show %s", __func__);
	return sprintf(buf, "%d\n", buffer_size);
}

static ssize_t dev_read_count_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("chrdev: sysfs show %s", __func__);
	return sprintf(buf, "%d\n", dev_read_count);
}

static ssize_t dev_write_count_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("chrdev: sysfs show %s", __func__);
	return sprintf(buf, "%d\n", dev_write_count);
}

static struct kobject *chrdev_kobj;
struct kobj_attribute data_buffer_attr = __ATTR(chrdev_buffer, 0660, data_buffer_show, NULL);
struct kobj_attribute data_buffer_size_attr = __ATTR(chrdev_buffer_size, 0660, buffer_size_show, NULL);
struct kobj_attribute dev_read_count_attr = __ATTR(chrdev_read_count, 0660, dev_read_count_show, NULL);
struct kobj_attribute dev_write_count_attr = __ATTR(chrdev_write_count, 0660, dev_write_count_show, NULL);

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
		pr_err("chrdev: already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("chrdev: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("chrdev: device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int ret;

	dev_read_count++;

	pr_info("chrdev: read from file %s\n", filep->f_path.dentry->d_iname);
	pr_info("chrdev: read from device %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));

	if (len > buffer_size)
		len = buffer_size;

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
		pr_err("chrdev: copy to user failed: %d\n", ret);
		return -EFAULT;
	}

	buffer_size = 0;

	pr_info("chrdev: %zu bytes read\n", len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int ret;

	dev_write_count++;

	pr_info("chrdev: write to file %s\n", filep->f_path.dentry->d_iname);
	pr_info("chrdev: write to device %d: %d\n", imajor(filep->f_inode), iminor(filep->f_inode));

	buffer_size = len;
	if (buffer_size > MAX_BUFFER_SIZE)
		buffer_size = MAX_BUFFER_SIZE;

	ret = copy_from_user(data_buffer, buffer, buffer_size);

	if (ret) {
		pr_err("chrdev: copy_from_user failed: %d\n", ret);
		return -EFAULT;
	}

	pr_info("chrdev: %d bytes written\n", buffer_size);
	return buffer_size;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int err = 0;

	pr_info("chrdev:  %s() cmd=0x%x arg=0x%lx\n", __func__, cmd, arg);

	if (_IOC_TYPE(cmd) != CDEV_IOC_MAGIC) {
		pr_err("chrdev: CHARDEV_IOC_MAGIC error\n");
		err = -ENOTTY;
	}

	if (_IOC_NR(cmd) >= CDEV_IOC_MAXNR) {
		pr_err("chrdev:  CHARDEV_IOC_MAXNR err\n");
		err = -ENOTTY;
	}

	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (err) {
		pr_err("chrdev:  _IOC_READ/_IOC_WRITE err\n");
		err = -EFAULT;
	}

	switch (_IOC_NR(cmd)) {
	case CLEAR_BUFFER:
		buffer_size = 0;
		data_buffer[0] = '\0';
		pr_info("chrdev:  CLEAR_BUFFER\n");
		break;
	case GET_BUFFER_SIZE:
		if (copy_to_user((int32_t *)arg, &buffer_size, sizeof(buffer_size))) {
			pr_err("chrdev:  GET_BUFFER_SIZE err\n");
			err = -ENOTTY;
		}
		pr_info("chrdev:  GET_BUFFER_SIZE buffer_size=%d\n", buffer_size);
		break;
	case SET_BUFFER_SIZE:
		if (copy_from_user(&buffer_size, (int32_t *)arg, sizeof(buffer_size))) {
			pr_err("chrdev:  SET_BUFFER_SIZE err\n");
			err = -ENOTTY;
		}
		if (buffer_size > MAX_BUFFER_SIZE)
			buffer_size = MAX_BUFFER_SIZE;
		pr_info("chrdev: buffer_size is %d\n", buffer_size);
		break;
	default:
		pr_warn("chrdev:  invalid cmd(%u)\n", cmd);
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

static int __init chrdev_init(void)
{
	dev_read_count = 0;
	dev_write_count = 0;
	is_open = 0;
	buffer_size = 0;
	dev = 0;
	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);

	if (major < 0) {
		pr_err("chrdev: register_chrdev failed: %d\n", major);
		return major;
	}
	pr_info("chrdev: register_chrdev ok, major = %d minor = %d\n", MAJOR(dev), MINOR(dev));
	cdev_init(&chrdev_cdev, &fops);

	if ((cdev_add(&chrdev_cdev, dev, 1)) < 0) {
		pr_err("chrdev: cannot add the device to the system\n");
		goto cdev_err;
	}
	pr_info("chrdev: cdev created successfully\n");

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass))
		goto class_err;

	pr_info("chrdev: device class created successfully\n");

	pdev = device_create(pclass, NULL, dev, NULL, CLASS_NAME"0");

	if (IS_ERR(pdev))
		goto device_err;

	pr_info("chrdev: device created successfully\n");

	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if (proc_folder == NULL) {
		pr_err("chrdev: error creating proc directory\n");
		goto proc_dir_err;
	}

	pr_info("chrdev: proc directory created successfully\n");

	proc_file = proc_create(PROC_FILE_NAME, 0644, proc_folder, &proc_fops);
	if (proc_file == NULL) {
		pr_err("chrdev: error creating proc file\n");
		goto proc_file_err;
	}
	pr_info("chrdev: proc file created successfully\n");

	chrdev_kobj = kobject_create_and_add("chrdev_sysfs", kernel_kobj);

	if (sysfs_create_file(chrdev_kobj, &data_buffer_attr.attr)) {
		pr_err("chrdev: cannot create sysfs file\n");
		goto sysfs_buffer_err;
	}

	if (sysfs_create_file(chrdev_kobj, &data_buffer_size_attr.attr)) {
		pr_err("chrdev: cannot create sysfs file\n");
		goto sysfs_buffer_size_err;
	}

	if (sysfs_create_file(chrdev_kobj, &dev_read_count_attr.attr)) {
		pr_err("chrdev: cannot create sysfs file\n");
		goto sysfs_read_count_err;
	}

	if (sysfs_create_file(chrdev_kobj, &dev_write_count_attr.attr)) {
		pr_err("chrdev: cannot create sysfs file\n");
		goto sysfs_write_count_err;
	}

	pr_info("chrdev: device driver insmod success\n");
	return 0;

sysfs_write_count_err:
	sysfs_remove_file(kernel_kobj, &dev_read_count_attr.attr);
sysfs_read_count_err:
	sysfs_remove_file(kernel_kobj, &data_buffer_size_attr.attr);
sysfs_buffer_size_err:
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
	kobject_put(chrdev_kobj);
	sysfs_remove_file(kernel_kobj, &dev_read_count_attr.attr);
	sysfs_remove_file(kernel_kobj, &data_buffer_size_attr.attr);
	sysfs_remove_file(kernel_kobj, &data_buffer_attr.attr);
	sysfs_remove_file(kernel_kobj, &dev_write_count_attr.attr);

	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("chrdev: module exited\n");

	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("HELLO: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
}

module_init(chrdev_init);
module_exit(chrdev_exit);
