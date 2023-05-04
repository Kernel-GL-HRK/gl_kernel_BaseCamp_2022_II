#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/printk.h>
#include "proc.h"

struct proc_dir_entry *proc_file;
struct proc_ops proc_fops = {
	.proc_read = proc_read,
};

MODULE_LICENSE("GPL");

int32_t create_procFS(void)
{
	proc_file = proc_create(PROC_FILE_NAME, ONLY_READ_MODE, NULL, &proc_fops);
	if(proc_file == NULL) {
		pr_err("servo-proc: can not create file proc: %s\n", PROC_FILE_NAME);
		return -ENOMEM;
	}
	pr_info("servo-proc: file proc %s, is created\n", PROC_FILE_NAME);
	return 0;
}

void remove_procFS(void)
{
	proc_remove(proc_file);
	pr_info("servo-proc: file proc %s, is removed\n", PROC_FILE_NAME);
}

ssize_t proc_read (struct file *filep, char *to_user, size_t len, loff_t *offs)
{
	return 0;
}