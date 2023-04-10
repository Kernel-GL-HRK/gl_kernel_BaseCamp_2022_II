#include <linux/string.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include "linux/kobject_ns.h"
#include "proc_ops.h"
#include "dev_ops.h"
#include "keypad.h"
#include "sys_ops.h"

MODULE_AUTHOR("Muravka Roman");
MODULE_DESCRIPTION("Simple charapter device - Matrix keypad");
MODULE_LICENSE("GPL");
MODULE_VERSION("2.0");

struct proc_ops proc_fops = {
	.proc_read = procFS_read,
};

struct class *class_tree;
struct device *device_file;
struct cdev device;
struct file_operations dev_fops;
dev_t MM;

struct proc_dir_entry *proc_file;
struct proc_dir_entry *proc_dir;
struct proc_ops proc_fops;
// Matrix variables

struct kobject *Dstate = NULL;
struct kobj_attribute Fstate = {
	.attr = {
		.name = FILE_SYSFS,
		.mode = 0666
	},
	.show = sysfs_show,
	.store = sysfs_store
};


struct file_operations dev_fops = {
		.open = dev_open,
		.release = dev_close,
		.read = dev_read,
		.write = dev_write,
};

static int __init matrix_init(void)
{
	// CHARAPTER
	if (alloc_chrdev_region(&MM, 0, 1, DEVICE_NAME) < 0) {
		pr_err("matrix: ERROR: no free Major-number");
		return -ENOMEM;
	}
	pr_info("matrix: free identifiers: Major = %d, Minor = %d;", MAJOR(MM), MINOR(MM));

	cdev_init(&device, &dev_fops);
	if (cdev_add(&device, MM, 1) < 0) {
		pr_err("matrix: ERROR: cannot add the device to the system\n");
		goto cdev_err;
	}
	pr_info("matrix: device added to the system");

	class_tree = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(class_tree)) {
		pr_err("matrix: ERROR: An error occurred while creating a class on the path /sys/class/%s" CLASS_NAME);
		goto class_err;
	}
	pr_info("matrix: device class on the path /sys/class/%s created succesfully\n", CLASS_NAME);

	device_file = device_create(class_tree, NULL, MM, NULL, DEVICE_NAME);
	if (IS_ERR(device_file)) {
		pr_err("matrix: ERROR: An error occurred while creating a device file on the path /dev/%s", DEVICE_NAME);
		goto device_err;
	}
	pr_info("matrix: device file on the path /dev/%s created succesfully\n", DEVICE_NAME);
	//SysFS
	Dstate = kobject_create_and_add(FOLDER_SYSFS, NULL);
	if (!Dstate) { 
		pr_err("matrix-sysfs: cannot create folder sysFS\n");
		goto sysfs_err;
	}
	
	if (sysfs_create_file(Dstate, &Fstate.attr)) {
		pr_err("matrix-sysfs: cannot create file sysFS\n");
		goto sysfs_err;
	}
	pr_info("matrix-sysfs: file /sys/%s/%s is created\n", FOLDER_SYSFS, FILE_SYSFS);
	// PROC
	proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_dir) {
		pr_err("matrix: ERROR: couldn`t initialize /proc/%s\n", PROC_DIR_NAME);
		return -ENOMEM;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0444, proc_dir, &proc_fops);
	if (!proc_file) {
		pr_err("matrix: ERROR: couldn`t initialize /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		goto proc_err;
	}
	pr_info("matrix: /proc/%s/%s created\n", PROC_DIR_NAME, PROC_FILE_NAME);
	matrix_key_setup();
	pr_calc(PR_GL_SEPARATOR);
	pr_calc(PR_START_CALC);
	matrix_check_key();
	pr_calc(PR_GL_SEPARATOR);
	output_choice_message();

	return 0;
// ERROR OPERATIONS
sysfs_err:
	sysfs_remove_file(Dstate, &Fstate.attr);
	kobject_put(Dstate);
proc_err:
	proc_remove(proc_dir);
device_err:
	device_destroy(class_tree, MM);
	class_destroy(class_tree);
class_err:
	cdev_del(&device);
cdev_err:
	unregister_chrdev_region(MM, 1);
	return -1;
}

static void __exit matrix_exit(void)
{
	uint8_t i;

	sysfs_remove_file(Dstate, &Fstate.attr);
	kobject_put(Dstate);
	device_destroy(class_tree, MM);
	class_destroy(class_tree);
	cdev_del(&device);
	unregister_chrdev_region(MM, 1);
	pr_info("matrix: device removed\n");

	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_info("matrix: procFS removed\n");

	for (i = 0; i < NUM_COL_ROW; i++) {
		gpio_unexport(rows[i]);
		gpio_unexport(cols[i]);
	}
	pr_info("matrix: gpio is unexported\n");
	pr_info("matrix: module exited\n");

}

module_init(matrix_init);
module_exit(matrix_exit);