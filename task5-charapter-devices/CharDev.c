#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/types.h>

MODULE_AUTHOR("Muravka Roman");
MODULE_DESCRIPTION("Simple charapter device - Matrix keypad");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");


#define MAX_BUFFER 1024

/*CHARAPTER DEFINES*/

//Charapter struct and variables
#define CLASS_NAME "Keypad"
#define DEVICE_NAME "Matrix"
static struct class *class_tree;
static struct device *device_file;
static struct cdev device;
dev_t MM = 0; 
//Charapter file operations
static int dev_open(struct inode *inodep, struct file *filep)
{return 0;}

static int dev_close(struct inode *inodep, struct file *filep)
{return 0;}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{return 0;}
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{return 0;}

static struct file_operations dev_fops = {
    .open = dev_open,
    .release = dev_close,
    .read = dev_read,
    .write = dev_write,
};


/*PROC DEFINES*/

//Proc struct and variables
#define PROC_DIR_NAME "Keypad_Matrix"
#define PROC_FILE_NAME "Driver_Read"
static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_dir;

//Proc file operations
static ssize_t proc_read(struct file *filep, char __user* buffer, size_t count, loff_t *offset)
{return 0;}

static struct proc_ops proc_fops = {
    .proc_read = proc_read,
};

static int matrix_init(void)
{
    //CHARAPTER
    if(alloc_chrdev_region(&MM,  0, 1, DEVICE_NAME) < 0){
        pr_err("matrix: ERROR: no free Major-number");
        return -ENOMEM;
    }
    pr_info("matrix: free identifiers: Major = %d, Minor = %d;", MAJOR(MM), MINOR(MM));

    cdev_init(&device, &dev_fops);
    if(cdev_add(&device, MM, 1) < 0){
        pr_err("matrix: ERROR: cannot add the device to the system\n");
        goto cdev_err;
    }
    pr_info("matrix: device added to the system");

    class_tree = class_create(THIS_MODULE, CLASS_NAME);
    if(IS_ERR(class_tree)){
        pr_err("matrix: ERROR: An error occurred while creating a class on the path /sys/class/%s" CLASS_NAME);
        goto class_err;
    }
    pr_info("matrix: device class on the path /sys/class/%s created succesfully\n", CLASS_NAME);

    device_file = device_create(class_tree, NULL, MM, NULL, DEVICE_NAME);
    if(IS_ERR(device_file)){
        pr_err("matrix: ERROR: An error occurred while creating a device file on the path /dev/%s", DEVICE_NAME);
        goto device_err;
    }
    pr_info("matrix: device file on the path /dev/%s created succesfully\n", DEVICE_NAME);

    //PROC
    proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
    if(!proc_dir){
        pr_err("matrix: ERROR: couldn`t initialize /proc/%s\n", PROC_DIR_NAME);
        return -ENOMEM;
    }
    
    proc_file = proc_create(PROC_FILE_NAME, 0444, proc_dir, &proc_fops);
    if(!proc_file){
        pr_err("matrix: ERROR: couldn`t initialize /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
        goto proc_err;
    }
    pr_info("matrix: /proc/%s/%s created\n", PROC_DIR_NAME, PROC_FILE_NAME);
    
    return 0;
//ERROR OPERATIONS
proc_err:
    proc_remove(proc_dir);
device_err:
    class_destroy:(class_tree);
class_err:
    cdev_del(&device);
cdev_err: 
    unregister_chrdev_region(MM, 1);
    return -1;
}

static void matrix_exit(void)
{
    device_destroy(class_tree, MM);
    class_destroy(class_tree);
    cdev_del(&device);
    unregister_chrdev_region(MM, 1);
    pr_info("matrix: device removed\n");

    proc_remove(proc_file);
    proc_remove(proc_file);
    pr_info("matrix: procFS removed\n");

    pr_info("matrix: module exited\n");
}

module_init(matrix_init);
module_exit(matrix_exit);