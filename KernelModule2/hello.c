// SPDX-License-Identifier: GPL-3.0-or-later

#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Laura Shcherbak");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux kernel module");  ///< The description -- see modinfo
MODULE_VERSION("1.0");              ///< The version of the module

#define PROC_BUFFER_SIZE 100

static char procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t procfs_buffer_size;

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

#define PROC_FILE_NAME "dummy"
#define PROC_DIR_NAME "hello"

static int v1;
module_param(v1, int, 0444); ///< Read permission bit for all users
MODULE_PARM_DESC(v1, "First value");  ///< parameter description

static int v2;
module_param(v2, int, 0444); ///< Read permission bit for all users
MODULE_PARM_DESC(v2, "Second value");

static ssize_t hello_read(struct file *File, char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (procfs_buffer_size == 0)
		return 0;

	to_copy = min(count, procfs_buffer_size);

	not_copied = copy_to_user(buffer, procfs_buffer, procfs_buffer_size);

	delta = to_copy - not_copied;
	procfs_buffer_size = delta;

	return delta;
}

static ssize_t hello_write(struct file *file, const char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	procfs_buffer_size = to_copy = min(count, PROC_BUFFER_SIZE);

	not_copied = copy_from_user(procfs_buffer, buffer, to_copy);

	delta = to_copy - not_copied;

	return delta;
}

//static struct proc_ops fops = {
static struct file_operations fops = {
	.read = hello_read,
	.write = hello_write
	//.proc_read = hello_read,
	//.proc_write = hello_write
};

static int __init hello_init(void)
{
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
	pr_info("HELLO: /proc/%s/%s created\n", PROC_DIR_NAME, PROC_FILE_NAME);
	pr_info("HELLO: Hello Kernel\n");
	pr_info("Sum: %d + %d = %d\n", v1, v2, v1+v2);
	pr_info("Substraction: %d - %d = %d\n", v1, v2, v1-v2);
	pr_info("Multiplication: %d * %d = %d\n", v1, v2, v1*v2);
	return 0;
}

static void __exit hello_exit(void)
{
	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("HELLO: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
	pr_info("HELLO: Goodbye Kernel\n");
}

module_init(hello_init);
module_exit(hello_exit);


