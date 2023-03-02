#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Serhii Shramko");
MODULE_DESCRIPTION("Simple module calculator with procfs");
MODULE_VERSION("0.1");

static int n1;
module_param(n1, int, 0);

static int n2;
module_param(n2, int, 0);

#define PROC_BUFFER_MAX_SIZE 512

static char procfs_buffer[PROC_BUFFER_MAX_SIZE] = { 0 };
static size_t procfs_buffer_size;

#define PROC_FILE_NAME "calc_module"
static struct proc_dir_entry *proc_file;

static ssize_t procfs_read_handler(struct file *File, char __user *buffer,
				   size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (*offset >= procfs_buffer_size)
		return 0;

	to_copy = min(count, procfs_buffer_size - (size_t)(*offset));

	not_copied = copy_to_user(buffer, procfs_buffer + (*offset), to_copy);

	delta = to_copy - not_copied;
	*offset += delta;

	return delta;
}

static struct proc_ops fops = { .proc_read = procfs_read_handler };

static bool write_one_operation(const char *fmt, int number1, int number2,
				int result)
{
	int need_size = snprintf(NULL, 0, fmt, number1, number2, result);

	if (procfs_buffer_size + need_size >= PROC_BUFFER_MAX_SIZE)
		return false;

	procfs_buffer_size += sprintf(procfs_buffer + procfs_buffer_size, fmt,
				      number1, number2, result);
	return true;
}

static void write_number_operations(void)
{
	if (!write_one_operation("Sum: %d + %d = %d\n", n1, n2, n1 + n2))
		return;
	if (!write_one_operation("Substraction: %d - %d = %d\n", n1, n2,
				 n1 - n2))
		return;
	if (!write_one_operation("Multiplication: %d * %d = %d\n", n1, n2,
				 n1 * n2))
		return;
}

static int __init calc_init(void)
{
	pr_info("calc_module: Module calculator start\n");

	proc_file = proc_create(PROC_FILE_NAME, 0644, NULL, &fops);
	if (!proc_file) {
		pr_err("calc_module: Error: Couldnot initialize /proc/%s\n",
		       PROC_FILE_NAME);
		proc_remove(proc_file);
		return -ENOMEM;
	}
	pr_info("calc_module: /proc/%s created\n", PROC_FILE_NAME);

	write_number_operations();
	return 0;
}

static void __exit calc_exit(void)
{
	proc_remove(proc_file);
	pr_info("calc_module: Module calculator exit\n");
}

module_init(calc_init);
module_exit(calc_exit);
