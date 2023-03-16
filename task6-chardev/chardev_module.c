#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/kobject.h>
#include <linux/ioctl.h>

#include "chardev_ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Serhii Shramko");
MODULE_DESCRIPTION("Simple chardev module with procfs");
MODULE_VERSION("0.1");

#define DATA_BUFFER_MAX_SIZE 1024

static char data_buffer[DATA_BUFFER_MAX_SIZE] = { 0 };
static size_t data_buffer_size;

static struct class *pclass;
static struct device *pdev;
static struct cdev chardev_dev;
dev_t dev;

static int major;
static bool is_open;

static int count_dev_read;
static int count_dev_write;

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

	count_dev_read++;

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

	count_dev_write++;

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

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int err = -1;

	pr_info("chardev_module: %s cmd=0x%x arg=0x%lx\n", __func__, cmd, arg);

	if (_IOC_TYPE(cmd) != CHARDEV_IOC_MAGIC) {
		pr_err("chardev_module: CHARDEV_IOC_MAGIC error\n");
		err = -ENOTTY;
		goto error_exit;
	}

	if (_IOC_NR(cmd) >= CHARDEV_IOC_MAXNR) {
		pr_err("chardev_module: CHARDEV_IOC_MAXNR error\n");
		err = -ENOTTY;
		goto error_exit;
	}

	if (_IOC_DIR(cmd)) {
		if (_IOC_DIR(cmd) & _IOC_READ)
			err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
		if (_IOC_DIR(cmd) & _IOC_WRITE)
			err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
		if (err) {
			pr_err("chardev_module: _IOC_READ/_IOC_WRITE error\n");
			err = -EFAULT;
			goto error_exit;
		}
	}

	switch (_IOC_NR(cmd)) {
	case CLEAR_BUFFER:
		data_buffer_size = 0;
		data_buffer[0] = '\0';
		pr_info("chardev_module: CLEAR_BUFFER\n");
		break;
	case GET_BUFFER_SIZE:
		if (copy_to_user((size_t *)arg, &data_buffer_size,
				 sizeof(data_buffer_size))) {
			pr_err("chardev_module: GET_BUFFER_SIZE error\n");
			err = -ENOTTY;
			goto error_exit;
		}
		pr_info("chardev_module: GET_BUFFER_SIZE data_buffer_size=%d\n",
			data_buffer_size);
		break;
	case SET_BUFFER_SIZE:
		if (copy_from_user(&data_buffer_size, (size_t *)arg,
				   sizeof(data_buffer_size))) {
			pr_err("chardev_module: SET_BUFFER_SIZE error\n");
			err = -ENOTTY;
			goto error_exit;
		}
		if (data_buffer_size > DATA_BUFFER_MAX_SIZE)
			data_buffer_size = DATA_BUFFER_MAX_SIZE;
		pr_info("chardev_module: SET_BUFFER_SIZE data_buffer_size=%d\n",
			data_buffer_size);
		break;
	default:
		pr_warn("chardev_module: invalid cmd(%u)\n", _IOC_NR(cmd));
	}

	return 0;

error_exit:
	return err;
}

static struct file_operations fops = { .open = dev_open,
				       .release = dev_realease,
				       .read = dev_read,
				       .write = dev_write,
				       .unlocked_ioctl = dev_ioctl };

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

static ssize_t data_buffer_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	pr_info("chardev_module: %s called\n", __func__);
	if (data_buffer_size == 0)
		return sprintf(buf, "\n");
	return sprintf(buf, "%s\n", data_buffer);
}

static ssize_t data_buffer_size_show(struct kobject *kobj,
				     struct kobj_attribute *attr, char *buf)
{
	pr_info("chardev_module: %s called\n", __func__);
	return sprintf(buf, "%d\n", data_buffer_size);
}

static ssize_t dev_read_count_show(struct kobject *kobj,
				   struct kobj_attribute *attr, char *buf)
{
	pr_info("chardev_module: %s called\n", __func__);
	return sprintf(buf, "%d\n", count_dev_read);
}

static ssize_t dev_write_count_show(struct kobject *kobj,
				    struct kobj_attribute *attr, char *buf)
{
	pr_info("chardev_module: %s called\n", __func__);
	return sprintf(buf, "%d\n", count_dev_write);
}

static struct kobject *chardev_kobj;
struct kobj_attribute data_buffer_attr =
	__ATTR(buffer, 0644, data_buffer_show, NULL);
struct kobj_attribute data_buffer_size_attr =
	__ATTR(buffer_size, 0644, data_buffer_size_show, NULL);
struct kobj_attribute dev_read_count_attr =
	__ATTR(dev_read_call_count, 0644, dev_read_count_show, NULL);
struct kobj_attribute dev_write_count_attr =
	__ATTR(dev_write_call_count, 0644, dev_write_count_show, NULL);

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
		pr_err("chardev_module: Could not initialize /proc/%s\n",
		       PROC_FILE_NAME);
		goto proc_create_exit;
	}

	chardev_kobj = kobject_create_and_add("chardev_sysfs", kernel_kobj);
	if (sysfs_create_file(chardev_kobj, &data_buffer_attr.attr)) {
		pr_err("chardev_module: cannot create data_buffer_attr file\n");
		goto sysfs_buffer_exit;
	}
	if (sysfs_create_file(chardev_kobj, &data_buffer_size_attr.attr)) {
		pr_err("chardev_module: cannot create data_buffer_size_attr file\n");
		goto sysfs_buffer_size_exit;
	}
	if (sysfs_create_file(chardev_kobj, &dev_read_count_attr.attr)) {
		pr_err("chardev_module: cannot create dev_read_count_attr file\n");
		goto sysfs_dev_read_exit;
	}
	if (sysfs_create_file(chardev_kobj, &dev_write_count_attr.attr)) {
		pr_err("chardev_module: cannot create dev_write_count_attr file\n");
		goto sysfs_dev_write_exit;
	}

	pr_info("chardev_module: Driver setup done\n");
	return 0;

sysfs_dev_write_exit:
	sysfs_remove_file(kernel_kobj, &dev_read_count_attr.attr);
sysfs_dev_read_exit:
	sysfs_remove_file(kernel_kobj, &data_buffer_size_attr.attr);
sysfs_buffer_size_exit:
	sysfs_remove_file(kernel_kobj, &data_buffer_attr.attr);
sysfs_buffer_exit:
	kobject_put(chardev_kobj);
	proc_remove(proc_file);
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
	sysfs_remove_file(kernel_kobj, &dev_write_count_attr.attr);
	sysfs_remove_file(kernel_kobj, &dev_read_count_attr.attr);
	sysfs_remove_file(kernel_kobj, &data_buffer_size_attr.attr);
	sysfs_remove_file(kernel_kobj, &data_buffer_attr.attr);
	kobject_put(chardev_kobj);
	proc_remove(proc_file);
	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chardev_dev);
	unregister_chrdev_region(dev, 1);
	pr_info("chardev_module: exit\n");
}

module_init(chardev_module_init);
module_exit(chardev_module_exit);
