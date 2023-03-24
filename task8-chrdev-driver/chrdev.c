#include <linux/kobject.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/err.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kriz");
MODULE_DESCRIPTION("Simple character device driver witn procfs and sysfs");
MODULE_VERSION("0.1");

#define CLASS_NAME  	"chrdev"
#define DEVICE_NAME 	"chrdev_simple"
#define BUFFER_SIZE 	 1024
#define PROC_BUFFER_SIZE 500
#define PROC_FILE_NAME   "dummy"
#define PROC_DIR_NAME    "chrdev"
#define PERMS 0644

static int is_open;
static int data_size;
static unsigned char data_buffer[BUFFER_SIZE];
static uint8_t procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t procfs_buffer_size = 0;

/*statistics for /proc and /sysfs*/
static uint64_t read_counts;
static uint64_t write_counts;
static uint64_t read_amount;
static uint64_t write_amount;

struct kobject *chrdev_kobj;
static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev; 
static struct proc_dir_entry *proc_file, *proc_folder;
dev_t dev = 0;					


static ssize_t proc_chrdev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
	pr_info("%s: read from file %s\n", DEVICE_NAME, filep->f_path.dentry->d_iname);
	pr_info("%s: read from device %d:%d\n", DEVICE_NAME, imajor(filep->f_inode), iminor(filep->f_inode));

	if (len > data_size) 
	{
		len = data_size; 
	}
	
	if (copy_to_user(buffer, data_buffer, len)) 
	{
		pr_err("%s: copy_to_user failed\n", DEVICE_NAME);
		return -EIO;
	}

	data_size = 0;
	pr_info("%s: %zu bytes read\n", DEVICE_NAME, len);

	read_counts++;
	read_amount += len;
	return len;
}

static struct proc_ops proc_fops = {
	.proc_read = proc_chrdev_read
};

static int dev_open (struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("%s: already open\n", DEVICE_NAME);
		return -EBUSY;
	}

	is_open = 1;
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
	pr_info("%s: read from file %s\n", DEVICE_NAME, filep->f_path.dentry->d_iname);
	pr_info("%s: read from device %d:%d\n", DEVICE_NAME, imajor(filep->f_inode), iminor(filep->f_inode));

	if (len > data_size) len = data_size; 

	
	if (copy_to_user(buffer, data_buffer, len)) {
		pr_err("%s: copy_to_user failed\n", DEVICE_NAME);
		return -EIO;
	}
	data_size = 0;
	pr_info("%s: %zu bytes read\n", DEVICE_NAME, len);

	read_counts++;
	read_amount += len;
	return len;
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
	int ret;

	pr_info("%s: write to file %s\n", DEVICE_NAME, filep->f_path.dentry->d_iname);
	pr_info("%s: write to device %d:%d\n", DEVICE_NAME, imajor(filep->f_inode), iminor(filep->f_inode));

	data_size = len;
	if(data_size > BUFFER_SIZE) data_size = BUFFER_SIZE;

	ret = copy_from_user(data_buffer, buffer, data_size);
	if(ret) {
		pr_err("%s: copy_from_user failed: %d\n", DEVICE_NAME, ret);
		return -EFAULT;
	}

	write_counts++;
	write_amount += len;
	pr_info("%s: %d bytes written\n", DEVICE_NAME, data_size);

	return data_size;
}

/*Callbacks provided by the driver*/
static struct file_operations chrdev_fops =
{
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

unsigned int sysfs_val = 0;

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	printk(KERN_INFO "Reading - sysfs show func...\n");
	return sprintf(buf,"%d\n%d\ndev_read() calls: %lld\ndev_write() calls: %lld\n",sysfs_val, BUFFER_SIZE, read_counts, write_counts);
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	printk(KERN_INFO "Writing - sysfs store function...\n");
	sscanf(buf, "%d", &sysfs_val);
	return count;
}

struct kobj_attribute chrdev_attr = __ATTR(chrdev_value, 0660, sysfs_show, sysfs_store);

static int __init chrdev_init(void) 
{
	is_open = 0;
	data_size = 0;

	/*Define major and minor*/
	if (alloc_chrdev_region (&dev, 3, 1, DEVICE_NAME)) {
		pr_err("%s: %s\n", DEVICE_NAME, "Failed to alloc_chrdev_region");
		return -1;
	}

	/*Add device to the system*/
	cdev_init(&chrdev_cdev, &chrdev_fops);
	if((cdev_add(&chrdev_cdev, dev, 1)) < 0) {
		pr_err("%s: %s\n", DEVICE_NAME, "Failed to add the device to the system");
		goto cdev_err;
	}

	/*Create device class*/
	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		goto class_err;
	}

	/*Create device node*/
	pdev = device_create(pclass, NULL, dev, NULL, CLASS_NAME"0");
	if (IS_ERR(pdev)) {
		goto device_err;
	}

	/* create procfs entry*/
	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if(IS_ERR(proc_folder))
	{
		pr_err("%s: %s\n", DEVICE_NAME, "Error: COULD not create proc_fs folder");
		goto procfs_folder_err;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_folder, &proc_fops);
	if(IS_ERR(proc_file))
	{
		pr_err("%s: %s\n", DEVICE_NAME, "Error: Could not initialize proc_fs file");
		goto procfs_file_err;
	}

	/*Create sysfs entry*/
	chrdev_kobj = kobject_create_and_add("chrdev_sysfs", kernel_kobj);

	if(sysfs_create_file(chrdev_kobj,&chrdev_attr.attr)) {
		pr_err("%s: %s\n", DEVICE_NAME, "Cannot create sysfs file\n");
		goto sysfs_err;
	}

	//pr_info("%s: major = %d, minor = %d\n", DEVICE_NAME, MAJOR(dev), MINOR(dev));
	pr_info("%s: %s\n", DEVICE_NAME, "INIT");

	return 0;
sysfs_err:
	sysfs_remove_file(kernel_kobj, &chrdev_attr.attr);
	kobject_put(chrdev_kobj);
procfs_file_err:
	proc_remove(proc_file);
procfs_folder_err:
	proc_remove(proc_folder);
device_err:
	class_destroy(pclass);
class_err:
	cdev_del(&chrdev_cdev);
cdev_err:
	unregister_chrdev_region(dev,1);
	return -1;
}

static void __exit chrdev_exit(void)
{
	sysfs_remove_file(kernel_kobj, &chrdev_attr.attr);
	kobject_put(chrdev_kobj);
	proc_remove(proc_file);
	proc_remove(proc_folder);
	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(dev,1);
	pr_info("%s: %s\n", DEVICE_NAME, "EXIT");
}

module_init(chrdev_init);
module_exit(chrdev_exit);