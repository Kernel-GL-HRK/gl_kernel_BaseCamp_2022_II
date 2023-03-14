#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device/class.h>
#include <linux/kobject.h>
#include <linux/string.h>

#define DEVICE_NAME "mymod"
#define PROC_NAME "mymod" 
#define DEVICE_CLASS "mymod_cdev"

#define PAGE 1024

#define PERMS 0644
/* cdev buffer */
#define BUFS PAGE * 4
static uint8_t dev_buf[BUFS] = {0}; // {[0 ... BUFS / 2 - 1] = '1', [BUFS / 2 ... BUFS-1] = '2'}; 
/* statistics to be printed in /proc and /sysfs */ 
static uint64_t read_counts;
static uint64_t write_counts;
static uint64_t read_amount;
static uint64_t write_amount;

/* proc_fs data */
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
	sprintf(proc_buf + 50, "CharDev Read/Write buffer size: %d\n", BUFS);
	sprintf(proc_buf + 100, "Reads: %llu\n", read_counts);
	sprintf(proc_buf + 150, "Read amount: %llu\n", read_amount);
	sprintf(proc_buf + 200, "Writes: %llu\n", write_counts);
	sprintf(proc_buf + 250, "Write amount: %llu\n", write_amount);
	
	if (copy_to_user(buf, proc_buf, PROC_SIZE))
		return -EIO;
		
	*pos += PROC_SIZE;
	pr_info("%s: %s %s %u %lld\n", DEVICE_NAME, __func__, "COPIED/FPOS", PROC_SIZE, *pos);
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

/* Define file operations (callbacks) provided by the driver */
static const struct file_operations mymod_cdev_fops = {
        .owner = THIS_MODULE,
        .open = open_cdev,
        .write = write_cdev,
        .read = read_cdev,
        .release = release_cdev,
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
	pr_info("%s: %s %s %lu %lld\n", DEVICE_NAME, __func__, "COPIED/FPOS", count, *pos);
	return count;
}

static ssize_t read_cdev(struct file *this_file, char __user *buf, size_t count, loff_t *pos)
{
	if (*pos >= BUFS)
		return 0;
	if (*pos + count > BUFS)
		count = BUFS - (*pos);
	if (copy_to_user(buf, dev_buf + (*pos), count))
		return -EIO;
	
	read_counts++;
	read_amount += count;
	*pos += count;
	pr_info("%s: %s %s %lu %lld\n", DEVICE_NAME, __func__, "COPIED/FPOS", count, *pos);
	return count;
}

static int release_cdev(struct inode *this_inode, struct file *this_file)
{
	/* clear blocking */
	dev_is_open = 0; 
	return 0;
}

/* device and  sysfs data */
#define SYSFS_SIZE  PAGE
//static uint8_t sysfs_buf[SYSFS_SIZE] = {0};

static struct class *mymod_class;
static struct device *mymode_device;
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
	sprintf(buf + 50, "CharDev Read/Write buffer size: %d\n", BUFS);
	sprintf(buf + 100, "Reads: %llu\n", read_counts);
	sprintf(buf + 150, "Read amount: %llu\n", read_amount);
	sprintf(buf + 200, "Writes: %llu\n", write_counts);
	sprintf(buf + 250, "Write amount: %llu\n", write_amount);
	sprintf(buf + 300, "First 256 bites of the cdev buf:\n");
	memcpy(buf + 350, dev_buf, 256);
	pr_info("%s: %s %s %u %u\n", DEVICE_NAME, __func__, "COPIED/FPOS", PAGE, PAGE);
	return PAGE;
}

static int __init init_function(void)
{
	
	pr_info("%s: %s\n", DEVICE_NAME, "INIT");
	
	/* Define MINOR and MAJOR of the device */
	if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME)) {
		pr_err("%s: %s\n", DEVICE_NAME, "Failed to alloc cdev region");
		return -1;
	}
	
	/* Create class of the device  */
	mymod_class = class_create(THIS_MODULE, DEVICE_CLASS);
	if (IS_ERR(mymod_class)) {
		pr_err("%s: %s\n", DEVICE_NAME, "Failed to create class");
		goto cdev_err;
	}
	/* init cdev by adding owner and fops and it to the system */
	cdev_init(&mymod_cdev, &mymod_cdev_fops);
	if (cdev_add(&mymod_cdev, dev_num, 1)) {
		pr_err("%s: %s\n", DEVICE_NAME, "Failed to register cdev");
		goto class_err;
	}
	
	/* device_create - creates a device and registers it with sysfs  */
	mymode_device = device_create(mymod_class, NULL, dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(mymode_device)) {
		pr_err("%s: %s\n", DEVICE_NAME, "Failed to create /dev entry");
		goto device_err;		
	}
	
	/* create procfs entry */
	proc_dir = proc_mkdir(PROC_NAME, NULL);
	if (IS_ERR(proc_dir)) {
		pr_err("%s: %s\n", DEVICE_NAME, "Failed to create proc_fs dir");
		goto proc_fs_dir__err;
	}
	proc_file = proc_create(PROC_NAME, PERMS, proc_dir, &pops);
	if (IS_ERR(proc_file)) {
		pr_err("%s: %s\n", DEVICE_NAME, "Failed to create proc_fs file");
		goto proc_fs_file__err;
	}
	
	cdev_kobj = kobject_create_and_add(DEVICE_CLASS, kernel_kobj);
	if (sysfs_create_file(cdev_kobj, &cdev_attr.attr)) {
		pr_err("%s: %s\n", DEVICE_NAME, "Failed to create sysfs file");
		goto sysfs_err;
	}
	
	pr_info("%s: %s\n", DEVICE_NAME, "Success");
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
	sysfs_remove_file(kernel_kobj, &cdev_attr.attr);
	kobject_put(cdev_kobj); 
	proc_remove(proc_file);
	proc_remove(proc_dir);
	device_destroy(mymod_class, dev_num);
	class_unregister(mymod_class);
	cdev_del(&mymod_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("%s: %s\n", DEVICE_NAME, "EXIT");
}


module_init(init_function);
module_exit(exit_function);

MODULE_AUTHOR("Yurii Klysko");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simle kernel module.");
