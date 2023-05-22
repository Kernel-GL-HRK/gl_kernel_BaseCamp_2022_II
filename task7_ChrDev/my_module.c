// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
/*
 * Kernel module
 * HomeTask: Character device with SysFS & ioctl
 */

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#include "cdev_ioctl.h"

MODULE_AUTHOR("Vadym Minziuk");
MODULE_DESCRIPTION("HomeTask: Character device driver with SysFS & ioctl");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.4");

#define BUFFER_SIZE 1024
#define PROC_BUFFER_SIZE 100
#define PROC_FILE_NAME "status"
#define PROC_DIR_NAME "chrdev"

static int num_read;
static int num_write;

static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev;
dev_t dev;

static int major;
static int is_open;

static int data_size;
static unsigned char data_buffer[BUFFER_SIZE];

static int32_t ioctl_val;

static char   procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t procfs_buffer_size;

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

static ssize_t chrdev_read(struct file *File, char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, have_copied;

	if (procfs_buffer_size == 0)
		return 0;

	//pr_info("chrdev: User spase want to copy = %d\n", count);
	to_copy = min_t(size_t, count, procfs_buffer_size);
	//pr_info("chrdev: to_copy = %d\n", to_copy);

	not_copied = copy_to_user(buffer, procfs_buffer, to_copy);
	//pr_info("chrdev: not_copied = %d\n", not_copied);

	have_copied = to_copy - not_copied;
	procfs_buffer_size = procfs_buffer_size - have_copied;

	return have_copied;
}

static struct proc_ops proc_fops = {
	.proc_read  = chrdev_read,
};

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("chrdev: already opened\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("chrdev: device has opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("chrdev: device has closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int ret;

	num_read = num_read + 1;
	pr_info("chrdev: read from file %s\n", filep->f_path.dentry->d_iname);
	pr_info("chrdev: read from device %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));

	if (len > data_size)
		len = data_size;

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
		pr_err("chrdev: copy_to_user failed: %d\n", ret);
		return -EFAULT;
	}
	data_size = 0; /*eof for cat*/

	pr_info("chrdev: %zu bytes read\n", len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int ret;

	num_write = num_write + 1;
	pr_info("chrdev: write to file %s\n", filep->f_path.dentry->d_iname);
	pr_info("chrdev: write to device %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));

	data_size = len;
	if (data_size > BUFFER_SIZE)
		data_size = BUFFER_SIZE;

	ret = copy_from_user(data_buffer, buffer, data_size);
	if (ret) {
		pr_err("chrdev: copy_from_user failed: %d\n", ret);
	return -EFAULT;
	}

	pr_info("chrdev: %d bytes written\n", data_size);
	return data_size;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int err = 0;

	pr_info("chrdev: %s() cmd=0x%x arg=0x%lx\n", __func__, cmd, arg);

	if (_IOC_TYPE(cmd) != CDEV_IOC_MAGIC) {
		pr_err("chrdev: CDEV_IOC_MAGIC err!\n");
		err = -ENOTTY;
		goto dev_ioctl_exit;
	}

	if (_IOC_NR(cmd) >= CDEV_IOC_MAXNR) {
		pr_err("chrdev: CDEV_IOC_MAXNR err!\n");
		err = -ENOTTY;
		goto dev_ioctl_exit;
	}

	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));

	if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));

	if (err) {
		pr_err("chrdev: _IOC_READ/_IOC_WRITE err!\n");
		err = -EFAULT;
		goto dev_ioctl_exit;
	}

	switch (cmd) {
	case CDEV_WR_VALUE:
		if (copy_from_user(&ioctl_val, (int32_t *) arg, sizeof(ioctl_val))) {
			pr_err("chrdev: data write : Err!\n");
			err = -EFAULT;
		}

		if (ioctl_val >= BUFFER_SIZE)
			ioctl_val = BUFFER_SIZE - 1;
		data_size = ioctl_val;

		pr_info("chrdev: SET_BUFFER_SIZE: BUFFER_SIZE = %d\n", ioctl_val);
		break;
	case CDEV_RD_VALUE:
		ioctl_val = data_size;
		if (copy_to_user((int32_t *) arg, &ioctl_val, sizeof(ioctl_val))) {
			pr_err("chrdev: data read : Err!\n");
			err = -EFAULT;
		}
		pr_info("chrdev: GET_BUFFER_SIZE: BUFFER_SIZE = %d\n", ioctl_val);
		break;
	case CDEV_CL_VALUE:
		if (copy_from_user(&ioctl_val, (int32_t *) arg, sizeof(ioctl_val))) {
			pr_err("chrdev: data write : Err!\n");
			err = -EFAULT;
		}
		pr_info("chrdev: CLEAR_BUFFER\n");
		sprintf(data_buffer, "%c", '\0');
		data_size = 0;
		break;
	default:
		pr_warn("chrdev: invalid cmd(%u)\n", cmd);
		err = -EFAULT;
		break;
	}
dev_ioctl_exit:
	return err;
}

static struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
	.unlocked_ioctl = dev_ioctl,
};

unsigned int sysfs_val;

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	//printk(KERN_INFO" Reading - sysfs show function...\n");
	pr_info("KERN_INFO: Reading - sysfs show function...\n");

	return sprintf(buf, "\ndata_size = %d\nmax_buf_size = %d\nnum_read = %d\nnum_write = %d\n", data_size, BUFFER_SIZE, num_read, num_write);
//	return sprintf(buf, "%d", sysfs_val);
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	//int rv;
	//printk(KERN_INFO" Writing - sysfs store function...\n");
	pr_info("KERN_INFO: Writing - sysfs store function...\n");

	//rv = sscanf(buf, "%d", &sysfs_val);
	//if (rv != 1)
	//	pr_info("Warning: sscanf return value problem.\n");
	if (kstrtoint(buf, 0, &sysfs_val))
		pr_info("Warning: problem with return value of kstrtoint.\n");

	return count;
}

struct kobject *chrdev_kobj;
struct kobj_attribute chrdev_attr = __ATTR(chrdev_value, 0660, sysfs_show, sysfs_store);

static int chrdev_init(void)
{
	int err_exit = 0;

	num_read = 0;
	num_write = 0;
	is_open = 0;
	data_size = 0;

	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);

	if (major < 0) {
		pr_err("chrdev: register_chrdev failed: %d", major);
		return major;
	}
	pr_info("chrdev: register_chrdev ok, major = %d minor = %d\n", MAJOR(dev), MINOR(dev));
	cdev_init(&chrdev_cdev, &fops);
	if ((cdev_add(&chrdev_cdev, dev, 1)) < 0) {
		pr_err("chrdev: cannot add the device to system\n");
		err_exit = -1;
		goto cdev_err;
	}
	pr_info("chrdev: cdev created successfully\n");

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		err_exit = -1;
		goto class_err;
	}
	pr_info("chrdev: device class created successfully\n");

	pdev = device_create(pclass, NULL, dev, NULL, CLASS_NAME"0");
	if (IS_ERR(pdev)) {
		err_exit = -1;
		goto device_err;
	}
	pr_info("chrdev: device node created successfully\n");

	/*Creating a directory in /sys/kernel/ */
	chrdev_kobj = kobject_create_and_add("chrdev_sysfs", kernel_kobj);

	/*Creating sysfs file for etx_value*/
	if (sysfs_create_file(chrdev_kobj, &chrdev_attr.attr)) {
		pr_err("chrdev: cannot create sysfs file\n");
		err_exit = -1;
		goto sysfs_err;
	}

	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_folder) {
		pr_err("chrdev: Error: Could not initialize /proc/%s folder\n", PROC_DIR_NAME);
		err_exit = -1;
		goto err_exit_1;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_folder, &proc_fops);
	if (!proc_file) {
		pr_err("chrdev: Error: Could not initialize file /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		err_exit = -2;
		goto err_exit_2;
	}

	pr_info("chrdev: /proc/%s/%s created\n", PROC_DIR_NAME, PROC_FILE_NAME);
	sprintf(procfs_buffer, "ChrDev BUFFER_SIZE = %d\n", BUFFER_SIZE);
	procfs_buffer_size = sizeof(procfs_buffer);

	pr_info("chrdev: device driver insmod success\n");
	return 0;

//err_exit_4:
//err_exit_3:
//	proc_remove(proc_file);
err_exit_2:
	proc_remove(proc_folder);
err_exit_1:
sysfs_err:
	sysfs_remove_file(kernel_kobj, &chrdev_attr.attr);
	kobject_put(chrdev_kobj);
device_err:
	class_destroy(pclass);
class_err:
	cdev_del(&chrdev_cdev);
cdev_err:
	unregister_chrdev_region(dev, 1);
	return err_exit;
}

static void chrdev_exit(void)
{
	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("chrdev: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
	sysfs_remove_file(kernel_kobj, &chrdev_attr.attr);
	kobject_put(chrdev_kobj);
	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(dev, 1);

	pr_info("chrdev: module exited\n");
}

module_init(chrdev_init);
module_exit(chrdev_exit);
