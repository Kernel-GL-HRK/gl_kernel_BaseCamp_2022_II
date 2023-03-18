// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kostiantynmakhno@gmail.com");
MODULE_DESCRIPTION("Simple character device driver");
MODULE_VERSION("1.0");

#define MAX_BUFF_LEN 1024UL
#define DRV_CLASS_NAME "chrdev"
#define DRV_DEVICE_NAME "chrdev0"
#define DRV_PROC_NAME  "chrdev"

struct device_data {
	struct cdev dev;
	const char *dev_name;
	size_t buff_len;
	char *buff;
};

struct driver_info {
	char *buff;
	size_t buff_len;
	size_t read_cnt;
	size_t write_cnt;
};

struct driver_data {
	struct proc_dir_entry *pfile;
	struct driver_info drv_info;
	struct class *pclass;
	struct device *pdevice;
	dev_t dev_num;
	struct device_data dev_data;
};

static char chrdev_buff[MAX_BUFF_LEN];

static struct driver_data drv_data = {
	.drv_info = {
		.buff      =  chrdev_buff,
		.buff_len  = MAX_BUFF_LEN,
		.read_cnt  = 0,
		.write_cnt = 0
	},
	.dev_data = {
		.dev_name = DRV_DEVICE_NAME,
		.buff_len = MAX_BUFF_LEN,
		.buff     = chrdev_buff
	}
};

static ssize_t
chrdev_buff_sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buff)
{
	return sprintf(buff, "%s", drv_data.drv_info.buff);
}

static ssize_t
chrdev_buff_len_sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buff)
{
	return sysfs_emit(buff, "%zu\n", drv_data.drv_info.buff_len);
}

static ssize_t
chrdev_read_cnt_sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buff)
{
	return sysfs_emit(buff, "%zu\n", drv_data.drv_info.read_cnt);
}

static ssize_t
chrdev_write_cnt_sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buff)
{
	return sysfs_emit(buff, "%zu\n", drv_data.drv_info.write_cnt);
}

struct kobject *chrdev_kobj;
struct kobj_attribute chrdev_buff_attr       = __ATTR(chrdev_buff, 0444, chrdev_buff_sysfs_show, NULL);
struct kobj_attribute chrdev_buff_len_attr   = __ATTR(chrdev_buff_len, 0444, chrdev_buff_len_sysfs_show, NULL);
struct kobj_attribute chrdev_read_cnt_attr   = __ATTR(chrdev_read_cnt, 0444, chrdev_read_cnt_sysfs_show, NULL);
struct kobj_attribute chrdev_write_cnt_attr  = __ATTR(chrdev_write_cnt, 0444, chrdev_write_cnt_sysfs_show, NULL);

ssize_t chrdev_read(struct file *filp, char __user *ubuf, size_t count, loff_t *off)
{
	size_t to_copy;
	struct device_data *dev_data;
	struct driver_data *drv_data;

	pr_info("%s: requested bytes from userspace %zu\n", __func__, count);
	dev_data = (struct device_data *)filp->private_data;
	drv_data = container_of(dev_data, struct driver_data, dev_data);
	to_copy = min(dev_data->buff_len - *off, count);

	if (to_copy == 0)
		return 0;

	drv_data->drv_info.read_cnt++;

	if (copy_to_user(ubuf, dev_data->buff + *off, to_copy))
		return -EFAULT;

	pr_info("%s: written bytes to userspace %zu\n", __func__, to_copy);
	*off += to_copy;

	return to_copy;
}

ssize_t chrdev_write(struct file *filp, const char __user *ubuf, size_t count, loff_t *off)
{
	size_t to_copy;
	struct device_data *dev_data;
	struct driver_data *drv_data;

	pr_info("%s: requested bytes from userspace %zu\n", __func__, count);
	dev_data = (struct device_data *)filp->private_data;
	drv_data = container_of(dev_data, struct driver_data, dev_data);
	to_copy = min(dev_data->buff_len - *off, count);

	if (to_copy == 0)
		return 0;

	drv_data->drv_info.write_cnt++;

	if (copy_from_user(dev_data->buff + *off, ubuf, to_copy))
		return -EFAULT;

	pr_info("%s: written bytes to the kernel space %zu\n", __func__, to_copy);
	*off += to_copy;

	return to_copy;
}

int chrdev_open(struct inode *inode, struct file *filp)
{
	struct device_data *dev_data;

	dev_data = container_of(inode->i_cdev, struct device_data, dev);
	filp->private_data = dev_data;

	pr_info("%s: file was opened\n", __func__);

	return 0;
}

int chrdev_release(struct inode *inode, struct file *filp)
{
	pr_info("%s: file was closed\n", __func__);
	return 0;
}

ssize_t	chrdev_proc_read(struct file *filp, char __user *ubuf, size_t count, loff_t *off)
{
	size_t to_copy;

	if (*off > 0)
		return 0;

	to_copy = min(drv_data.dev_data.buff_len, count);

	if (copy_to_user(ubuf, drv_data.dev_data.buff, to_copy))
		return -EFAULT;

	pr_info("%s: sent bytes to user %zu\n", __func__, to_copy);
	*off += to_copy;

	return to_copy;
}

static const struct file_operations fops = {
	.owner   = THIS_MODULE,
	.open    = chrdev_open,
	.release = chrdev_release,
	.read    = chrdev_read,
	.write   = chrdev_write
};

static const struct proc_ops chrdev_ops = {
	.proc_read    = chrdev_proc_read
};

static int __init chrdev_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&drv_data.dev_num, 0, 1, "chrdev_device");
	if (ret < 0)
		goto err_alloc_dev_num;

	pr_info("%s: new device number was allocated major:minor %u:%u\n",
		__func__, MAJOR(drv_data.dev_num), MINOR(drv_data.dev_num));

	cdev_init(&drv_data.dev_data.dev, &fops);
	drv_data.dev_data.dev.owner = THIS_MODULE;

	ret = cdev_add(&drv_data.dev_data.dev, drv_data.dev_num, 1);
	if (ret < 0)
		goto err_cdev_add;

	pr_info("%s: init character device in VFS\n", __func__);

	drv_data.pclass = class_create(THIS_MODULE, DRV_CLASS_NAME);
	if (IS_ERR(drv_data.pclass)) {
		ret = PTR_ERR(drv_data.pclass);
		goto err_class_create;
	}

	drv_data.pdevice = device_create(drv_data.pclass, NULL, drv_data.dev_num,
					NULL, drv_data.dev_data.dev_name);
	if (IS_ERR(drv_data.pdevice)) {
		ret = PTR_ERR(drv_data.pdevice);
		goto err_dev_create;
	}

	drv_data.pfile = proc_create(DRV_PROC_NAME, 0666, NULL, &chrdev_ops);
	if (drv_data.pfile == NULL)
		pr_err("%s: can not create /proc/%s file\n", __func__, DRV_PROC_NAME);

	chrdev_kobj = kobject_create_and_add("chrdev_sysfs", kernel_kobj);
	if (chrdev_kobj == NULL) {
		pr_err("%s: Can not create kobject\n", __func__);
		goto err_kobj;
	}

	if (sysfs_create_file(chrdev_kobj, &chrdev_buff_attr.attr)) {
		pr_err("%s: Can not create buff attribute\n", __func__);
		goto err_buff_attr;
	}

	if (sysfs_create_file(chrdev_kobj, &chrdev_buff_len_attr.attr)) {
		pr_err("%s: Can not create buff_len attribute\n", __func__);
		goto err_buff_len_attr;
	}

	if (sysfs_create_file(chrdev_kobj, &chrdev_read_cnt_attr.attr)) {
		pr_err("%s: Can not create read_cnt attribute\n", __func__);
		goto err_read_cnt_attr;
	}

	if (sysfs_create_file(chrdev_kobj, &chrdev_write_cnt_attr.attr)) {
		pr_err("%s: Can not create write_cnt attribute\n", __func__);
		goto err_write_cnt_attr;
	}

	pr_info("%s: driver %s was successfully inserted\n", __func__, drv_data.dev_data.dev_name);

	return 0;

err_write_cnt_attr:
	sysfs_remove_file(chrdev_kobj, &chrdev_read_cnt_attr.attr);
err_read_cnt_attr:
	sysfs_remove_file(chrdev_kobj, &chrdev_buff_len_attr.attr);
err_buff_len_attr:
	sysfs_remove_file(chrdev_kobj, &chrdev_buff_attr.attr);
err_buff_attr:
	kobject_put(chrdev_kobj);
err_kobj:
	device_destroy(drv_data.pclass, drv_data.dev_num);
err_dev_create:
	class_destroy(drv_data.pclass);
err_class_create:
	cdev_del(&drv_data.dev_data.dev);
err_cdev_add:
	unregister_chrdev_region(drv_data.dev_num, 1);
err_alloc_dev_num:
	return ret;
}

static void __exit chrdev_exit(void)
{
	sysfs_remove_file(chrdev_kobj, &chrdev_write_cnt_attr.attr);
	sysfs_remove_file(chrdev_kobj, &chrdev_read_cnt_attr.attr);
	sysfs_remove_file(chrdev_kobj, &chrdev_buff_len_attr.attr);
	sysfs_remove_file(chrdev_kobj, &chrdev_buff_attr.attr);
	kobject_put(chrdev_kobj);
	proc_remove(drv_data.pfile);
	device_destroy(drv_data.pclass, drv_data.dev_num);
	class_destroy(drv_data.pclass);
	cdev_del(&drv_data.dev_data.dev);
	unregister_chrdev_region(drv_data.dev_num, 1);
	pr_info("%s: chrdev device was unload\n", __func__);
}

module_init(chrdev_init);
module_exit(chrdev_exit);
