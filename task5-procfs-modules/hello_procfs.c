#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#define procfs_name "helloworld"

static struct proc_dir_entry *our_proc_file;

static ssize_t procfile_read(struct file *filePointer, char __user *buffer,
			     size_t buffer_length, loff_t *offset)
{
	char s[13] = "HelloWorld!\n";
	int len = sizeof(s);
	ssize_t ret = len;

	if (*offset >= len || copy_to_user(buffer, s, len)) {
		pr_info("copy_to_user failed\n");
		ret = 0;
	} else {
		pr_info("procfile read %s\n", filePointer->f_path.dentry->d_name.name);
		*offset += len;
	}
	return ret;
}

static const struct proc_ops proc_file_fops = {
	.proc_read = procfile_read,
};

static int __init hello_procfs_init(void)
{
	our_proc_file = proc_create(procfs_name, 0644, NULL, &proc_file_fops);
	if (our_proc_file == NULL) {
		proc_remove(our_proc_file);
		pr_alert("Error:Could not initialize /proc/%s\n", procfs_name);
		return -ENOMEM;
	}
	pr_info("/proc/%s created\n", procfs_name);
	return 0;
}

static void __exit hello_procfs_exit(void)
{
	proc_remove(our_proc_file);
	pr_info("/proc/%s removed\n", procfs_name);
}

module_init(hello_procfs_init);
module_exit(hello_procfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii");
MODULE_DESCRIPTION("Read-only /proc File");
