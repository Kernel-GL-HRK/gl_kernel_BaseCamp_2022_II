#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/err.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Serhii Shramko");
MODULE_DESCRIPTION("Simple chardev module with procfs");
MODULE_VERSION("0.1");

#define DATA_BUFFER_MAX_SIZE 1024

static char data_buffer[DATA_BUFFER_MAX_SIZE] = { 0 };
static size_t data_buffer_size;

#define CLASS_NAME "chardev"
#define DEVICE_NAME "chardev_device"

static struct class *pclass;
static struct device *pdev;
static struct cdev chardev_dev;
dev_t dev;

static int major;
static bool is_open;

static int dev_open(struct inode *inode, struct file *file)
{
	if (is_open) {
		pr_err("chardev_module: already opened\n");
		return -EBUSY;
	}

	is_open = true;
	pr_info("chardev_module: device is opened\n");
	return 0;
}

static int dev_realease(struct inode *inode, struct file *file)
{
	is_open = false;
	pr_info("chardev_module: device is dev_realeased\n");
	return 0;
}

static ssize_t dev_read(struct file *file, char *buffer, size_t len,
			loff_t *offset)
{
	ssize_t not_copied;

	pr_info("chardev_module: read from file %s\n",
		file->f_path.dentry->d_iname);
	pr_info("chardev_module: read from device %d:%d\n",
		imajor(file->f_inode), iminor(file->f_inode));

	if (len > data_buffer_size)
		len = data_buffer_size;

	not_copied = copy_to_user(buffer, data_buffer, len);
	if (not_copied) {
		pr_err("chardev_module: copy_to_user failed %d\n", not_copied);
		return -EFAULT;
	}
	data_buffer_size = 0;

	pr_info("chardev_module: %d bytes read\n", len);
	return len;
}

static ssize_t dev_write(struct file *file, const char *buffer, size_t len,
			 loff_t *offset)
{
	ssize_t not_copied;

	pr_info("chardev_module: write to file %s\n",
		file->f_path.dentry->d_iname);
	pr_info("chardev_module: write to device %d:%d\n",
		imajor(file->f_inode), iminor(file->f_inode));

	data_buffer_size = len;
	if (data_buffer_size > DATA_BUFFER_MAX_SIZE)
		data_buffer_size = DATA_BUFFER_MAX_SIZE;

	not_copied = copy_from_user(data_buffer, buffer, data_buffer_size);
	if (not_copied) {
		pr_err("chardev_module: copy_to_user failed %d\n", not_copied);
		return -EFAULT;
	}

	pr_info("chardev_module: %d bytes written\n", len);
	return len;
}

static struct file_operations fops = { .open = dev_open,
				       .release = dev_realease,
				       .read = dev_read,
				       .write = dev_write };

#define PROC_FILE_NAME "chardev_module"
static struct proc_dir_entry *proc_file;

static ssize_t procfs_read_handler(struct file *File, char __user *buffer,
				   size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (*offset >= data_buffer_size)
		return 0;

	to_copy = min(count, data_buffer_size - (size_t)(*offset));

	not_copied = copy_to_user(buffer, data_buffer + (*offset), to_copy);

	delta = to_copy - not_copied;
	*offset += delta;

	return delta;
}

static struct proc_ops proc_ops = { .proc_read = procfs_read_handler };

static int __init chardev_module_init(void)
{
	pr_info("chardev_module: init\n");
	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);

	if (major < 0) {
		pr_err("chardev_module: alloc_chrdev_region failed\n");
		return major;
	}
	pr_info("chardev_module: alloc_chrdev_region success %d:%d\n",
		MAJOR(dev), MINOR(dev));

	cdev_init(&chardev_dev, &fops);
	if (cdev_add(&chardev_dev, dev, 1) < 0) {
		pr_err("chardev_module: cdev_add failed\n");
		goto cdev_add_exit;
	}
	pr_info("chardev_module: cdev_add success\n");

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass))
		goto class_create_exit;
	pr_info("chardev_module: class_create success\n");

	pdev = device_create(pclass, NULL, dev, NULL, CLASS_NAME "0");
	if (IS_ERR(pdev))
		goto device_create_exit;
	pr_info("chardev_module: device_create success\n");

	proc_file = proc_create(PROC_FILE_NAME, 0644, NULL, &proc_ops);
	if (!proc_file) {
		pr_err("chardev_module: Error: Could not initialize /proc/%s\n",
		       PROC_FILE_NAME);
		goto proc_create_exit;
	}

	pr_info("chardev_module: Driver setup done\n");
	return 0;

proc_create_exit:
	device_destroy(pclass, dev);
device_create_exit:
	class_destroy(pclass);
class_create_exit:
	cdev_del(&chardev_dev);
cdev_add_exit:
	unregister_chrdev_region(dev, 1);
	return -1;
}

static void __exit chardev_module_exit(void)
{
	proc_remove(proc_file);
	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chardev_dev);
	unregister_chrdev_region(dev, 1);
	pr_info("chardev_module: exit\n");
}

module_init(chardev_module_init);
module_exit(chardev_module_exit);
