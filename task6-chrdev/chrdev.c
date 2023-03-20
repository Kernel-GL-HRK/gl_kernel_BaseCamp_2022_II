// SPDX-License-Identifier: GPL
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergiy Us");
MODULE_DESCRIPTION("A simple character device driver - procfs, chrdev");
MODULE_VERSION("0.1");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define	HAVE_PROC_OPS
#endif

#define BUFFER_SIZE		1024
#define PROC_DIR_NAME		"chrdir"	/* /proc/chrdir/chrdev */
#define PROC_FILE_NAME		"chrdev"

#define CLASS_NAME		"chrdev"
#define DEVICE_NAME		"chrdev0"

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_dir;
static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev;

static dev_t dev;
static int major;
static int is_open;
static int data_size;
static size_t procfs_buffer_size;
static size_t procfs_buffer_size_tmp;
static unsigned char data_buffer[BUFFER_SIZE];

static int dev_open(struct inode *inodep, struct file *filep);
static int dev_release(struct inode *inodep, struct file *filep);
static ssize_t dev_read(struct file *filep, char *buffer,
			size_t len, loff_t *offset);
static ssize_t dev_write(struct file *filep, const char *buffer,
			 size_t len, loff_t *offset);

static ssize_t proc_read(struct file *file, char __user *buffer,
			 size_t count, loff_t *offset);


static const struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read  = dev_read,
	.write = dev_write,
};

#ifdef HAVE_PROC_OPS
static const struct proc_ops fops1 = {
	.proc_read = proc_read,
	//.proc_write = dev_write,
};
#else
static const struct file_operations fops1 = {
	.read  = proc_read,
};
#endif

static int __init chrdev_init(void)
{
	/* procfs init */
	proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_dir) {
		pr_err("PROC: ERROR: Could not create /proc/%s directory\n",
		       PROC_DIR_NAME);
		return -ENOMEM;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_dir, &fops1);
	if (!proc_file) {
		pr_err("PROC: ERROR: Could not initialize /proc/%s/%s\n",
		       PROC_DIR_NAME, PROC_FILE_NAME);
		proc_remove(proc_file);
		proc_remove(proc_dir);
		return -ENOMEM;
	}

	pr_info("PROC: /proc/%s/%s created\n",
		PROC_DIR_NAME, PROC_FILE_NAME);

	/* chardev init */
	is_open = 0;
	data_size = 0;

	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);

	if (major < 0) {
		pr_err("CHRDEV: ERROR: register chardev failed: %d\n", major);
		return major;
	}

	pr_info("CHRDEV: register chardev ok, major = %d, minor = %d\n",
		MAJOR(dev), MINOR(dev));

	cdev_init(&chrdev_cdev, &fops);
	if (cdev_add(&chrdev_cdev, dev, 1) < 0) {
		pr_err("CHRDEV: ERROR: cannot add the device to the system\n");
		goto cdev_err;
	}
	pr_info("CHRDEV: cdev created successfully\n");

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		goto class_err;
	}
	pr_info("CHRDEV: device class created successfully\n");

	pdev = device_create(pclass, NULL, dev, NULL, DEVICE_NAME);
	if (IS_ERR(pdev)) {
		goto device_err;
	}
	pr_info("CHRDEV: device node created successfully\n");

	return 0;

device_err:
	class_destroy(pclass);
class_err:
	cdev_del(&chrdev_cdev);
cdev_err:
	unregister_chrdev_region(dev, 1);
	return -1;
}

static void __exit chrdev_exit(void)
{
	/* chardev */
	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("CHRDEV: /dev/%s removed\n", DEVICE_NAME);

	/* procfs */
	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_info("PROC: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
}

module_init(chrdev_init);
module_exit(chrdev_exit);


static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("CHRDEV: already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("CHRDEV: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("CHRDEV: device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer,
			size_t len, loff_t *offset)
{
	int ret;

	pr_info("CHRDEV: read from file %s\n", filep->f_path.dentry->d_iname);
	pr_info("CHRDEV: read from device %d:%d\n",
		imajor(filep->f_inode), iminor(filep->f_inode));

	if (len > data_size) {
		len = data_size;
	}

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
		pr_err("CHRDEV: ERROR: copy_to_user failed: %d\n", ret);
		return -EFAULT;
	}
	data_size = 0;		/* eof for cat */

	// proc test
	procfs_buffer_size = 0;
	procfs_buffer_size_tmp = 0;

	pr_info("CHRDEV: %zu bytes read\n", len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer,
			 size_t len, loff_t *offset)
{
	int ret;

	pr_info("CHRDEV: write to file %s\n", filep->f_path.dentry->d_iname);
	pr_info("CHRDEV: write to device %d:%d\n",
		imajor(filep->f_inode), iminor(filep->f_inode));

	data_size = len;
	if (data_size > BUFFER_SIZE) {
		data_size = BUFFER_SIZE;
	}

	ret = copy_from_user(data_buffer, buffer, data_size);
	if (ret) {
		pr_err("CHRDEV: ERROR: copy_from_user failed: %d\n", ret);
		return -EFAULT;
	}

	pr_info("CHRDEV: %d bytes written\n", data_size);

	procfs_buffer_size = (size_t)data_size;
	procfs_buffer_size_tmp = procfs_buffer_size;

	return data_size;
}

static ssize_t proc_read(struct file *file, char __user *buffer,
			  size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (!procfs_buffer_size) {
		pr_info("PROC: %zu bytes read\n", procfs_buffer_size);
		procfs_buffer_size = procfs_buffer_size_tmp;
		return 0;
	}
	to_copy = min(count, procfs_buffer_size);
	not_copied = copy_to_user(buffer, data_buffer, to_copy);

	delta = to_copy - not_copied;
	procfs_buffer_size -= delta;
	pr_info("PROC: %zu bytes read\n", delta);

	return delta;
}

