// SPDX-License-Identifier: GPL-2.0+

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Yurii Bezkor");
MODULE_DESCRIPTION("Proc module");
MODULE_VERSION("0.1");

#define PROC_BUFFER_SIZE 100

static char procfs_buffer[PROC_BUFFER_SIZE] = { 0 };
static size_t procfs_buffer_size;

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_dir;

#define PROC_FILE_NAME "hello"
#define PROC_DIR_NAME "bezkor"

static ssize_t procfs_read(struct file *file, char __user *buffer,
			   size_t count_bytes, loff_t *offset)
{
	pr_info("Read info from procfs\n");

	if (*offset > 0 || count_bytes < procfs_buffer_size) {
		pr_err("Illegal buffer\n");
		return 0;
	}

	if (copy_to_user(buffer, procfs_buffer, procfs_buffer_size)) {
		pr_err("Error copy_to_user\n");
		return -EFAULT;
	};
	*offset = procfs_buffer_size;
	return procfs_buffer_size;
}

static ssize_t procfs_write(struct file *file, const char __user *buffer,
			    size_t count_bytes, loff_t *offset)
{
	pr_info("Write info to procfs\n");

	if (*offset > 0 || count_bytes > PROC_BUFFER_SIZE) {
		pr_err("Illegal buffer\n");
		return -EFAULT;
	}

	if (copy_from_user(procfs_buffer, buffer, count_bytes)) {
		pr_err("Error copy_from_user\n");
		return -EFAULT;
	};

	procfs_buffer_size = count_bytes;
	pr_info("Buffer is %s\n", procfs_buffer);

	return procfs_buffer_size;
}

static const struct file_operations fops = {
				.owner = THIS_MODULE,
				.read = procfs_read,
				.write = procfs_write };

static int __init hello_init(void)
{
	proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (proc_dir == NULL) {
		pr_err("PROC module: Could not create /proc/%s/ folder !\n",
		       PROC_DIR_NAME);
		return -ENOMEM;
	}
	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_dir, &fops);
	if (proc_file == NULL) {
		pr_err("PROC module: Could not create /proc/%s/%s !\n",
		       PROC_DIR_NAME, PROC_FILE_NAME);
		proc_remove(proc_file);
		proc_remove(proc_dir);
		return -ENOMEM;
	}
	procfs_buffer_size = 0;
	pr_info("PROC module: Hello /proc/%s/%s !\n", PROC_DIR_NAME,
		PROC_FILE_NAME);
	return 0;
}

static void __exit hello_exit(void)
{
	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_info("PROC module: Goodbye /proc/%s/%s !\n", PROC_DIR_NAME,
		PROC_FILE_NAME);
}

module_init(hello_init);
module_exit(hello_exit);
