// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h> // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h> // Core header for loading LKMs into the kernel
#include <linux/kernel.h> // Contains types, macros, functions for the kernel
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

MODULE_LICENSE("GPL"); ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Maksym Khomenko"); ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Simple gpio driver with proc_fs support"); ///< The description -- see modinfo
MODULE_VERSION("0.2"); ///< The version of the module

#define PROC_BUFFER_SIZE 256

static char procfs_buffer[PROC_BUFFER_SIZE] = "MODULE NAME: my_module\nAUTHOR: Maksym Khomenko\nDESCRIPTION: Simple gpio driver with proc_fs support\nVERSION: 0.2\n";
static size_t procfs_buffer_size = PROC_BUFFER_SIZE;
static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

#define PROC_FILE_NAME "my_info"
#define PROC_DIR_NAME "my_module" /* /proc/my_module/my_state */

static ssize_t my_module_read(struct file *File, char __user* buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (procfs_buffer_size == 0) 
		{
			procfs_buffer_size = strlen(procfs_buffer);
			return 0;
		}

	to_copy = min(count, procfs_buffer_size);
	not_copied = copy_to_user(buffer, procfs_buffer, to_copy);
	delta = to_copy - not_copied;
	procfs_buffer_size -= delta;
	return delta;
}

//static struct file_operations fops = {.read = my_module_read};
static struct proc_ops fops = {.proc_read = my_module_read};


static int __init my_module_init(void)
{
	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if(!proc_folder)
	{
		pr_err("MY_MODULE: Error Could not create /proc/%s/ folder\n", PROC_DIR_NAME);
		return -ENOMEM;
	}
	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_folder, &fops);
	if(!proc_file)
	{
		pr_err("MY_MODULE: Error Could not initialize /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		proc_remove(proc_file);
		proc_remove(proc_folder);
		return -ENOMEM;
	}
procfs_buffer_size = strlen(procfs_buffer);
pr_info("MY_MODULE: /proc/%s/%s created\n", PROC_DIR_NAME, PROC_FILE_NAME);
return 0;
}

static void __exit my_module_exit(void)
{
	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("MY_MODULE: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
}

module_init(my_module_init);
module_exit(my_module_exit);
