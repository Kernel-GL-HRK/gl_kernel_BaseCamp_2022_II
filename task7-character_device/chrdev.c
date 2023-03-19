#include <linux/init.h>         // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>       // Core header for loading LKMs into the kernel
#include <linux/kernel.h>       // Contains types, macros, functions for the kernel
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/err.h>


MODULE_LICENSE("GPL");                  ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Yevhen Kolesnyk");       ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Character device driver with Proc Interface");  ///< The description -- see modinfo
MODULE_VERSION("0.1");                  ///< The version of the module


#define BUFFER_SIZE 1024


static char procfs_buffer[BUFFER_SIZE] = {0};
static size_t procfs_buffer_size;

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

#define PROC_FILE_NAME "hello_info"
#define PROC_DIR_NAME "hello_folder"

#define CLASS_NAME "chrdev"
#define DEVICE_NAME "chrdev_example"


static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev;
dev_t dev = 0;

static int major;
static int is_open;

static int data_size = 0;
static unsigned char data_buffer[BUFFER_SIZE];


static ssize_t proc_fs_read(struct file *File, char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (procfs_buffer_size == 0)
		return 0;

	to_copy = min(count, procfs_buffer_size);

	not_copied = copy_to_user(buffer, procfs_buffer, to_copy);

	delta = to_copy - not_copied;
	procfs_buffer_size -= delta;

	return delta;
}


static struct proc_ops proc_fops = {
	.proc_read = proc_fs_read,

};

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("chrdev: already open\n"); 
		return -EBUSY;
	}

	is_open = 1; 
	pr_info("chrdev: device opened\n");
	return 0;
}	


static int dev_release(struct inode *inodep, struct file *filep) 
{
	is_open = 0; 
	pr_info("chrdev: device closed\n"); 
	return 0;
}

 static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
 {
	int ret;


	pr_info("chrdev: read from file %s\n", filep->f_path.dentry->d_iname); 
	pr_info("chrdev: read from device %d:%d\n", imajor(filep->f_inode), iminor (filep->f_inode));

	if (len > data_size) len = data_size;

	ret = copy_to_user(buffer, data_buffer, len); 
	if (ret) { 
		pr_err("chrdev: copy to user failed: %d\n", ret); 
		return -EFAULT;
	}

	data_size = 0; 

	pr_info("chrdev: %zu bytes read\n", len);
	return len;
 }

 static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int ret;

	
	data_size = len;
	if (data_size> BUFFER_SIZE) data_size = BUFFER_SIZE;

	ret = copy_from_user(data_buffer, buffer, data_size);

	if (ret) {
		pr_err("chrdev: copy_from_user failed: %d\n", ret); 
		return -EFAULT;
	}


	procfs_buffer_size = sprintf(procfs_buffer, data_buffer);

	memcpy(procfs_buffer, data_buffer, data_size);
	procfs_buffer_size = data_size;



	pr_info("chrdev: %d bytes written\n", data_size);
	return data_size;
}

static struct file_operations fops =
{
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};




static int __init chrdev_init(void)
{
    is_open = 0;
    data_size = 0;
    major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);

    if (major < 0) {
        pr_err("chrdev: register_chrdev failed: %d\n", major);
        return major;
    }
    pr_info("chrdev: register_chrdev ok, major = %d minor = %d\n", MAJOR(dev), MINOR(dev));
    cdev_init(&chrdev_cdev, &fops);

    if ((cdev_add(&chrdev_cdev, dev, 1)) < 0) {
        pr_err("chrdev: cannot add the device to the system\n");
        goto cdev_err;
    }
    pr_info("chrdev: cdev created successfully\n");

    pclass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(pclass)) {
        goto class_err;
    }

    pr_info("chrdev: device class created successfully\n");

    pdev = device_create(pclass, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(pdev)) {
        goto device_err;
    }
    pr_info("chrdev: device created successfully\n");

    proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
    if (proc_folder == NULL) {
        pr_err("chrdev: error creating proc directory\n");
        goto proc_dir_err;
    }
    pr_info("chrdev: proc directory created successfully\n");

    proc_file = proc_create(PROC_FILE_NAME, 0644, proc_folder, &proc_fops);
    if (proc_file == NULL) {
        pr_err("chrdev: error creating proc file\n");
        goto proc_file_err;
    }
    pr_info("chrdev: proc file created successfully\n");

    return 0;

proc_file_err:
    remove_proc_entry(PROC_FILE_NAME, proc_folder);
proc_dir_err:
    remove_proc_entry(PROC_DIR_NAME, NULL);
device_err:
    device_destroy(pclass, dev);
class_err:
    class_destroy(pclass);
cdev_err:
    unregister_chrdev_region(dev, 1);
    return -EFAULT;
}

static void __exit chrdev_exit(void)
{
	device_destroy (pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region (dev, 1);
	pr_info("chrdev: module exited\n");

	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("HELLO: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
}

module_init(chrdev_init);
module_exit(chrdev_exit);
