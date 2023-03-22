// SPDX-License-Identifier: GPL
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/err.h>
#include <linux/uaccess.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergiy Us");
MODULE_DESCRIPTION("A simple character device driver - procfs, sysfs, chrdev");
MODULE_VERSION("0.1");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define	HAVE_PROC_OPS
#endif

#define BUFFER_SIZE		1024
#define PROC_DIR_NAME		"chrdir"	/* /proc/chrdir/chrdev */
#define PROC_FILE_NAME		"chrdev"

#define CLASS_NAME		"chrdev"
#define DEVICE_NAME		"chrdev0"	/* /dev/chrdev0 */

#define SYS_DIR_NAME		"chrdir"	/* /sys/chrdir */
#define SYS_FILE_NAME		"chrdev"	/* /sys/chrdir/chrdev */

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
static unsigned int dev_read_count;
static unsigned int dev_write_count;
static unsigned char data_buffer[BUFFER_SIZE];

static int dev_open(struct inode *inodep, struct file *filep);
static int dev_release(struct inode *inodep, struct file *filep);
static ssize_t dev_read(struct file *filep, char *buffer,
			size_t len, loff_t *offset);
static ssize_t dev_write(struct file *filep, const char *buffer,
			 size_t len, loff_t *offset);
static ssize_t proc_read(struct file *file, char __user *buffer,
			 size_t count, loff_t *offset);
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf);

struct kobject *chrdev_kobj;
struct kobj_attribute chrdev_attr = __ATTR(chrdev, 0660, sysfs_show, NULL);

static const struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read  = dev_read,
	.write = dev_write,
};

#ifdef HAVE_PROC_OPS
static const struct proc_ops fops1 = {
	.proc_read = proc_read,
};
#else
static const struct file_operations fops1 = {
	.read  = proc_read,
};
#endif

static int __init chrdev_init(void)
{
	int ret;

	/* procfs init */
	proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_dir) {
		pr_err("CHRDEV: PROC: ERROR: Could not create /proc/%s directory\n",
		       PROC_DIR_NAME);
		ret = -ENOMEM;
		goto proc_dir_err;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_dir, &fops1);
	if (!proc_file) {
		pr_err("CHRDEV: PROC: ERROR: Could not initialize /proc/%s/%s\n",
		       PROC_DIR_NAME, PROC_FILE_NAME);
		ret = -ENOMEM;
		goto proc_file_err;
	}
	pr_info("CHRDEV: PROC: /proc/%s/%s created\n",
		PROC_DIR_NAME, PROC_FILE_NAME);

	/* sysfs init */
	/* /sys/kernel/SYS_DIR_NAME */
	/* if use NULL - /sys/SYS_DIR_NAME/ */
	chrdev_kobj = kobject_create_and_add(SYS_DIR_NAME, kernel_kobj);

	/* Creating sysfs file for etx value */
	if (sysfs_create_file(chrdev_kobj, &chrdev_attr.attr)) {
		pr_err("CHRDEV: SYS: ERROR: cannot create sysfs file\n");
		ret = -EACCES;
		goto sysfs_err;
	}
	pr_info("CHRDEV: SYS: /sys/kernel/%s/%s created\n",
		SYS_DIR_NAME, SYS_FILE_NAME);

	/* chardev init */
	is_open = 0;
	data_size = 0;

	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);

	if (major < 0) {
		pr_err("CHRDEV: DEV: ERROR: register chardev failed: %d\n",
		       major);
		ret = -EEXIST;
		goto cdev_major_err;
	}

	pr_info("CHRDEV: DEV: register chardev ok, major = %d, minor = %d\n",
		MAJOR(dev), MINOR(dev));

	cdev_init(&chrdev_cdev, &fops);
	if (cdev_add(&chrdev_cdev, dev, 1) < 0) {
		pr_err("CHRDEV: DEV: ERROR: ");
		pr_err("cannot add the device to the system\n");
		ret = -ENODEV;
		goto cdev_err;
	}
	pr_info("CHRDEV: DEV: character device created successfully\n");

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		ret = -EBADF;
		goto class_err;
	}
	pr_info("CHRDEV: DEV: device class created successfully\n");

	pdev = device_create(pclass, NULL, dev, NULL, DEVICE_NAME);
	if (IS_ERR(pdev)) {
		ret = -EBUSY;
		goto device_err;
	}
	pr_info("CHRDEV: DEV: /dev/%s created\n", DEVICE_NAME);
	return 0;

device_err:
	class_destroy(pclass);
class_err:
	cdev_del(&chrdev_cdev);
cdev_err:
	unregister_chrdev_region(dev, 1);
cdev_major_err:
sysfs_err:
	sysfs_remove_file(kernel_kobj, &chrdev_attr.attr);
	kobject_put(chrdev_kobj);
proc_file_err:
	proc_remove(proc_file);
	proc_remove(proc_dir);
proc_dir_err:
	return ret;
}

static void __exit chrdev_exit(void)
{
	/* chardev */
	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("CHRDEV: DEV: /dev/%s removed\n", DEVICE_NAME);

	/* sysfs */
	sysfs_remove_file(kernel_kobj, &chrdev_attr.attr);
	kobject_put(chrdev_kobj);
	pr_info("CHRDEV: SYS: /sys/kernel/%s/%s removed\n",
		SYS_DIR_NAME, SYS_FILE_NAME);

	/* procfs */
	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_info("CHRDEV: PROC: /proc/%s/%s removed\n",
		PROC_DIR_NAME, PROC_FILE_NAME);
}

module_init(chrdev_init);
module_exit(chrdev_exit);


static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("CHRDEV: DEV: already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("CHRDEV: DEV: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("CHRDEV: DEV: device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer,
			size_t len, loff_t *offset)
{
	int ret;

	/* number of calls */
	++dev_read_count;

	pr_info("CHRDEV: DEV: read from file %s\n",
		filep->f_path.dentry->d_iname);
	pr_info("CHRDEV: DEV: read from device %d:%d\n",
		imajor(filep->f_inode), iminor(filep->f_inode));

	if (len > data_size) {
		len = data_size;
	}

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
		pr_err("CHRDEV: DEV: ERROR: copy_to_user failed: %d\n", ret);
		return -EFAULT;
	}
	data_size = 0;		/* eof for cat */
	pr_info("CHRDEV: DEV: %zu bytes read\n", len);

	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer,
			 size_t len, loff_t *offset)
{
	int ret;

	/* number of calls */
	++dev_write_count;

	pr_info("CHRDEV: DEV: write to file %s\n",
		filep->f_path.dentry->d_iname);
	pr_info("CHRDEV: DEV: write to device %d:%d\n",
		imajor(filep->f_inode), iminor(filep->f_inode));

	data_size = len;
	if (data_size > BUFFER_SIZE) {
		data_size = BUFFER_SIZE;
	}

	ret = copy_from_user(data_buffer, buffer, data_size);
	if (ret) {
		pr_err("CHRDEV: DEV: ERROR: copy_from_user failed: %d\n", ret);
		return -EFAULT;
	}
	pr_info("CHRDEV: DEV: %d bytes written\n", data_size);

	return data_size;
}

static ssize_t proc_read(struct file *file, char __user *buffer,
			  size_t count, loff_t *offset)
{
	static unsigned int cnt;
	ssize_t to_copy, not_copied, delta;

	if (cnt == 0) {
		procfs_buffer_size = (size_t)data_size;
	}

	if (!procfs_buffer_size) {
		pr_info("CHRDEV: PROC: %zu bytes read\n", procfs_buffer_size);
		procfs_buffer_size = (size_t)data_size;
		cnt = 0;
		return 0;
	}
	to_copy = min(count, procfs_buffer_size);
	not_copied = copy_to_user(buffer, data_buffer, to_copy);

	delta = to_copy - not_copied;
	procfs_buffer_size -= delta;
	++cnt;
	pr_info("CHRDEV: PROC: %zu bytes read\n", delta);

	return delta;
}

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
#define S_BUFFER_SIZE	256

	size_t i = 0;
	size_t j = 0;
	unsigned char s_buf[S_BUFFER_SIZE];

	pr_info("CHRDEV: SYS: reading %u bytes\n", data_size);

	/* create service buffer */
	snprintf(s_buf, sizeof(s_buf) - 1,
		 "devfs: buffer (used)	= %u bytes\n"
		 "devfs: buffer (total)	= %u bytes\n"
		 "devfs: calls (read)	= %u\n"
		 "devfs: calls (write)	= %u\n",
		 data_size, BUFFER_SIZE, dev_read_count, dev_write_count);

	/* copy data_buffer to sysfs buf */
	if (data_size > 0) {
		for (i = 0; i < data_size; ++i) {
			buf[i] = data_buffer[i];
		}
	}

	/* add service buffer to sysfs buf */
	while ((s_buf[j] != '\0') && (j + 1 < sizeof(s_buf))) {
		buf[i] = s_buf[j];
		++i;
		++j;
	}

	return i;
}

