#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device/class.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include "mymod_uapi.h"

/* device and class */
static struct class *mymod_class;
static struct device *mymode_device;

#define PAGE 1024


#define PERMS 0644
/* cdev buffer */
static uint32_t BUFS = PAGE * 4;
static uint8_t * dev_buf; // {[0 ... BUFS / 2 - 1] = '1', [BUFS / 2 ... BUFS-1] = '2'}; 
/* actual amount of data is written to buffer */
static uint32_t dev_buf_amount;
/* statistics to be printed in /proc and /sysfs */ 
static uint64_t read_counts;
static uint64_t write_counts;
static uint64_t read_amount;
static uint64_t write_amount;


/* proc_fs data */
#define PROC_NAME "mymod" 
#define PROC_SIZE  PAGE
static uint8_t proc_buf[PROC_SIZE] = {0};

static struct proc_dir_entry *proc_dir, *proc_file;
static ssize_t proc_read(struct file *filp, char __user *buf, size_t count, loff_t *pos);

static struct proc_ops pops = {
	.proc_read = proc_read,
};

static ssize_t proc_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	if (*pos > 0)
		return 0;
		
	sprintf(proc_buf, "Module name: %s\n", DEVICE_NAME);
	sprintf(proc_buf + 20, "CharDev Read/Write buffer size: %d\n", BUFS);
	sprintf(proc_buf + 60, "Current amount of data in the buffer: %d\n", dev_buf_amount);
	sprintf(proc_buf + 120, "Reads: %llu\n", read_counts);
	sprintf(proc_buf + 150, "Read amount: %llu\n", read_amount);
	sprintf(proc_buf + 200, "Writes: %llu\n", write_counts);
	sprintf(proc_buf + 250, "Write amount: %llu\n", write_amount);
	
	if (copy_to_user(buf, proc_buf, PROC_SIZE))
		return -EIO;
		
	*pos += PROC_SIZE; 
	dev_info(mymode_device, "%s %s %d %lld", __func__, "COPIED/FPOS", PROC_SIZE, *pos);
	return PROC_SIZE;
}

/* cdev data */

static struct cdev mymod_cdev;
static dev_t dev_num;
static uint32_t dev_is_open;

/* file operations callbacks  for cdev */

static int open_cdev(struct inode *, struct file *);
static ssize_t write_cdev(struct file *, const char __user *, size_t, loff_t *);
static ssize_t read_cdev(struct file *, char __user *, size_t, loff_t *);
static int release_cdev(struct inode *, struct file *);
static long ioctl_cdev(struct file *, unsigned int, unsigned long);

/* Define file operations (callbacks) provided by the driver */
static const struct file_operations mymod_cdev_fops = {
        .owner = THIS_MODULE,
        .open = open_cdev,
        .write = write_cdev,
        .read = read_cdev,
        .release = release_cdev,
        .unlocked_ioctl = ioctl_cdev,
};

static int open_cdev(struct inode *this_inode, struct file *this_file) 
{
	if (dev_is_open)
		return -EBUSY;
	
	dev_is_open = 1;
	return 0;
}

static ssize_t write_cdev(struct file *this_file, const char __user *buf, size_t count, loff_t *pos)
{
	
	if (*pos >= BUFS)
		return 0;
	if (*pos + count > BUFS)
		count = BUFS - (*pos);
	if (copy_from_user(dev_buf + (*pos), buf, count))
		return -EIO;
		
	*pos += count;
	write_counts++;
	write_amount += count;
	
	/*  *pos is a real  size of the written data. It updates at each write run */
	
	dev_buf_amount = *pos;
	dev_info(mymode_device, "%s %s %lu %lld", __func__, "COPIED/FPOS", count, *pos);
	return count;
}

static ssize_t read_cdev(struct file *this_file, char __user *buf, size_t count, loff_t *pos)
{
	/* Use dev_buf_amount instead DUFS to read real size of data. */ 
	if (*pos >= dev_buf_amount)
		return 0;
	if (*pos + count > dev_buf_amount)
		count = dev_buf_amount - (*pos);
	if (copy_to_user(buf, dev_buf + (*pos), count))
		return -EIO;
	
	read_counts++;
	read_amount += count;
	*pos += count; 
	dev_info(mymode_device, "%s %s %lu %lld", __func__, "COPIED/FPOS", count, *pos);
	return count;
}

static int release_cdev(struct inode *this_inode, struct file *this_file)
{
	/* clear blocking */
	dev_is_open = 0; 
	return 0;
}

static long ioctl_cdev(struct file *this_file, unsigned int cmd, unsigned long arg)
{
	dev_info(mymode_device, "%s 0x%x 0x%lx", __func__, cmd, arg);
	switch (cmd) {
		case MYMOD_CLEAR_BUFFER:
		 	dev_info(mymode_device, "Clear dev_buf");
		 	dev_buf_amount = 0;
			break;
		case MYMOD_GET_BUFFER_SIZE:
			dev_info(mymode_device, "Return dev_buf size");
			put_user(BUFS, &arg);
			break;
		case MYMOD_SET_BUFFER_SIZE:
			dev_info(mymode_device, "Set dev_buf size");
			BUFS = arg;
			dev_buf = krealloc(dev_buf, BUFS, GFP_KERNEL);
			if (IS_ERR(dev_buf)) {
				dev_err(mymode_device, "%s", "Failed to resize dev_buf");
				return -ENOMEM;
			}
			break;
		default:
			return -ENOTTY;
	}
	
	return 0;
}
/* device and  sysfs data */
#define SYSFS_SIZE  PAGE
//static uint8_t sysfs_buf[SYSFS_SIZE] = {0};

static struct kobject *cdev_kobj;
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char __user *buf);
static struct kobj_attribute cdev_attr = {
	.attr = {
		.name = DEVICE_NAME,
		.mode = 0666,
	},
	.show = sysfs_show,
};

/* Define file operations (callbacks) provided by sysfs kobj */
/*
static const struct sysfs_ops mymod_sysfs_ops = {
        .show = sysfs_show
};
*/

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char __user *buf)
{ 
	if (access_ok(buf, SYSFS_SIZE))
		return -EIO;
	
	sprintf(buf, "Module name: %s\n", DEVICE_NAME);
	sprintf(buf + 20, "CharDev Read/Write buffer size: %d\n", BUFS);
	sprintf(buf + 60, "Current amount of data in the buffer: %d\n", dev_buf_amount);
	sprintf(buf + 120, "Reads: %llu\n", read_counts);
	sprintf(buf + 150, "Read amount: %llu\n", read_amount);
	sprintf(buf + 200, "Writes: %llu\n", write_counts);
	sprintf(buf + 250, "Write amount: %llu\n", write_amount);
	sprintf(buf + 300, "First 256 bites of the cdev buf:\n");
	memcpy(buf + 350, dev_buf, dev_buf_amount);
	dev_info(mymode_device, "%s %s %u %u", __func__, "COPIED/FPOS", PAGE, PAGE);
	return PAGE;
}

static int __init init_function(void)
{
	
	pr_info("%s: %s\n", DEVICE_NAME, "Init start");
	
	
	/* Define MINOR and MAJOR of the device */
	if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME)) {
		dev_err(mymode_device, "%s", "Failed to alloc cdev region");
		return -1;
	}
	
	/* Create class of the device  */
	mymod_class = class_create(THIS_MODULE, DEVICE_CLASS);
	if (IS_ERR(mymod_class)) {
		dev_err(mymode_device, "%s", "Failed to create class");
		goto cdev_err;
	}
	/* init cdev by adding owner and fops and it to the system */
	cdev_init(&mymod_cdev, &mymod_cdev_fops);
	if (cdev_add(&mymod_cdev, dev_num, 1)) {
		dev_err(mymode_device, "%s", "Failed to register cdev");
		goto class_err;
	}
	
	/* device_create - creates a device and registers it with sysfs  */
	mymode_device = device_create(mymod_class, NULL, dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(mymode_device)) {
		dev_err(mymode_device, "%s", "Failed to create /dev entry");
		goto device_err;		
	}
	
	/* create procfs entry */
	proc_dir = proc_mkdir(PROC_NAME, NULL);
	if (IS_ERR(proc_dir)) {
		dev_err(mymode_device, "%s", "Failed to create proc_fs dir");
		goto proc_fs_dir__err;
	}
	proc_file = proc_create(PROC_NAME, PERMS, proc_dir, &pops);
	if (IS_ERR(proc_file)) {
		dev_err(mymode_device, "%s", "Failed to create proc_fs file");
		goto proc_fs_file__err;
	}
	
	cdev_kobj = kobject_create_and_add(DEVICE_CLASS, kernel_kobj);
	if (sysfs_create_file(cdev_kobj, &cdev_attr.attr)) {
		dev_err(mymode_device, "%s", "Failed to create sysfs file");
		goto sysfs_err;
	}
	
	dev_buf = kmalloc(BUFS, GFP_KERNEL);
	if (IS_ERR(dev_buf)) {
		dev_err(mymode_device, "%s", "Failed to alloc dev_buf");
		return -ENOMEM;
	}

	dev_info(mymode_device,"%s", " Init Success");
	return 0;
	
sysfs_err:
	sysfs_remove_file(kernel_kobj, &cdev_attr.attr);
	kobject_put(cdev_kobj); 
proc_fs_file__err:
	proc_remove(proc_file);
proc_fs_dir__err:
	proc_remove(proc_dir);
	
device_err:
	class_unregister(mymod_class);
class_err:
	cdev_del(&mymod_cdev);
cdev_err:
	unregister_chrdev_region(dev_num, 1);
	return -1;

}

static void __exit exit_function(void)
{
	kfree(dev_buf);
	dev_info(mymode_device,"%s", "Exit start");
	sysfs_remove_file(kernel_kobj, &cdev_attr.attr);
	kobject_put(cdev_kobj); 
	proc_remove(proc_file);
	proc_remove(proc_dir);
	device_destroy(mymod_class, dev_num);
	class_unregister(mymod_class);
	cdev_del(&mymod_cdev);
	unregister_chrdev_region(dev_num, 1);  
	
	pr_info("%s: %s\n", DEVICE_NAME, "Exit success");
}


module_init(init_function);
module_exit(exit_function);

MODULE_AUTHOR("Yurii Klysko");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simle kernel module.");
