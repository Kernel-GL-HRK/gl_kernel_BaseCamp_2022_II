#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device/class.h>

#define DEVICE_NAME "mymod"
#define PROC_NAME "mymod" 
#define DEVICE_CLASS "mymod_cdev"

#define PERMS 0644
#define BUFS 1024 * 4
static char modbuf[BUFS] = {[0 ... BUFS / 2 - 1] = '1', [BUFS / 2 ... BUFS-1] = '2'};

/* proc_fs data */
static struct proc_dir_entry *proc_dir, *proc_file;
static ssize_t mymod_read(struct file *filp, char __user *buf, size_t count, loff_t *pos);

static struct proc_ops pops = {
	.proc_read = mymod_read,
};

static ssize_t mymod_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	if (*pos >= BUFS)
		return 0;
	if (*pos + count > BUFS)
		count = BUFS - (*pos);
	if (copy_to_user(buf, modbuf + (*pos), count))
		return -EIO;

	*pos += count;
	pr_info("%s: %s %lu %lld\n", DEVICE_NAME, "COPIED/FPOS", count, *pos);
	return count;
}

/* cdev data */
static struct cdev mymod_cdev;
static dev_t dev_num;

/* file operations callbacks  for cdev */

static int open_mymod_cdev(struct inode *, struct file *);
static ssize_t write_mymod_cdev(struct file *, const char __user *, size_t, loff_t *);
static ssize_t read_mymod_cdev(struct file *, char __user *, size_t, loff_t *);
static int release_mymod_cdev(struct inode *, struct file *);

/* Define file operations (callbacks) provided by the driver */
static const struct file_operations mymod_cdev_fops = {
        .owner = THIS_MODULE,
        .open = open_mymod_cdev,
        .write = write_mymod_cdev,
        .read = read_mymod_cdev,
        .release = release_mymod_cdev,
};

static int open_mymod_cdev(struct inode *, struct file *) 
{
	return 0;
}
static ssize_t write_mymod_cdev(struct file *, const char __user *, size_t, loff_t *)
{
	return 0;
}
static ssize_t read_mymod_cdev(struct file *, char __user *, size_t, loff_t *)
{
	return 0;
}
static int release_mymod_cdev(struct inode *, struct file *)
{
	return 0;
}

/* device and  sysfs data */
struct class * mymod_class;
struct device * mymode_device;


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
	
	pr_info("%s: %s\n", DEVICE_NAME, "Success");
	return 0;
	

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
