#include <linux/init.h>         // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>       // Core header for loading LKMs into the kernel
#include <linux/kernel.h>       // Contains types, macros, functions for the kernel
#include <linux/moduleparam.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>


MODULE_LICENSE("GPL");                  ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Yevhen Kolesnyk");       ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Simple Calculator Module with Proc Interface");  ///< The description -- see modinfo
MODULE_VERSION("0.2");                  ///< The version of the module

static int num1;
static int num2;

module_param(num1, int, 0660);
module_param(num2, int, 0660);

MODULE_PARM_DESC(num1, "First value");
MODULE_PARM_DESC(num2, "Second value");

#define PROC_BUFFER_SIZE 100

static char procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t procfs_buffer_size;

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

#define PROC_FILE_NAME "hello_info"
#define PROC_DIR_NAME "hello_folder"


static ssize_t hello_read(struct file *File, char __user *buffer, size_t count, loff_t *offset)
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

static struct proc_ops fops = {
	.proc_read = hello_read,

};


static int __init hello_init(void)
{
procfs_buffer_size = sprintf(procfs_buffer, "%d + %d = %d\n", num1, num2, num1 + num2);
procfs_buffer_size += sprintf(procfs_buffer + strlen(procfs_buffer), "%d - %d = %d\n", num1, num2, num1 - num2);
procfs_buffer_size += sprintf(procfs_buffer + strlen(procfs_buffer), "%d * %d = %d\n", num1, num2, num1 * num2);

pr_info("HELLO: Hello Kernel!\n%s", procfs_buffer);

proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
if (!proc_folder) {
	pr_err("HELLO: Error: Could not create /proc/%s/ folder\n", PROC_DIR_NAME);
	return -ENOMEM;
}
proc_file = proc_create(PROC_FILE_NAME, 0666, proc_folder, &fops);
if (!proc_file) {
	pr_err("HELLO: Error: Could not initialize /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
	proc_remove(proc_file);
	proc_remove(proc_folder);
	return -ENOMEM;
}

return 0;
}


static void __exit hello_exit(void)
{
pr_info("HELLO: Goodbye Kernel!\n");

proc_remove(proc_file);
proc_remove(proc_folder);
pr_info("HELLO: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
}

module_init(hello_init);
module_exit(hello_exit);
