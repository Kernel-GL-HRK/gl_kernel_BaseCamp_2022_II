#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kriz");
MODULE_DESCRIPTION("Hello_world module with simple calculator (sum, sub and mul)");
MODULE_VERSION("0.1");

#define PROC_BUFFER_SIZE 100

static char   procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t procfs_buffer_size = 0; 

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

#define PROC_FILE_NAME "dummy"
#define PROC_DIR_NAME  "hello"

static ssize_t hello_read(struct file *File, char __user* buffer, size_t count, loff_t *offset)
{
	int ret;

	if (*offset > 0)
		ret = 0;

	if (copy_to_user(buffer, procfs_buffer, procfs_buffer_size))
		return -EIO;

	*offset += procfs_buffer_size;
	ret = procfs_buffer_size;

	return ret;
}

static struct proc_ops fops = {
	.proc_read = hello_read
};

static int op1 = -1;
module_param(op1, int, 00644);
MODULE_PARM_DESC(op1, "First operator for calculation");

static int op2 = -1;
module_param(op2, int, 00644);
MODULE_PARM_DESC(op2, "Second operator for calculation");

static int __init hello_init(void) {
	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if(!proc_folder)
	{
		pr_err("HELLO: Error: COULD not create /proc/%s/ folder\n", PROC_DIR_NAME);
		return -ENOMEM;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_folder, &fops);
	if(!proc_file)
	{
		pr_err("HELLO: Error: Could not initialize /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		proc_remove(proc_folder);
		return -ENOMEM;
	}

	pr_info("HELLO: /proc/%s/%s created\n", PROC_DIR_NAME, PROC_FILE_NAME);
	pr_info("HELLO: Hello, world!\nCalculations:\n");
	pr_info("HELLO:Sum: %d + %d = %d\n", op1, op2, op1 + op2);
	pr_info("HELLO:Sub: %d - %d = %d\n", op1, op2, op1 - op2);
	pr_info("HELLO:Mul: %d * %d = %d\n", op1, op2, op1 * op2);
	return 0;
}

static void __exit hello_exit(void)
{
	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("HELLO: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
	pr_info("HELLO: Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);
