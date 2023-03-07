#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#define DEVICE_NAME "mymod"
#define PROC_NAME "mymod"
#define PERMS 0644
#define BUFS 1024 * 4
static char modbuf[BUFS] = {[0 ... BUFS / 2 - 1] = '1', [BUFS / 2 ... BUFS-1] = '2'};
static struct proc_dir_entry *proc_dir, *proc_file;
static ssize_t mymod_read(struct file *filp, char __user *buf, size_t count, loff_t *pos);

static struct proc_ops pops = {
	.proc_read = mymod_read,
};

static ssize_t mymod_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	if (*pos >= BUFS)
		return 0;
	if (*pos + count > BUFS)
		count = BUFS - (*pos);
	if (copy_to_user(buf, modbuf + (*pos), count))
		return -EIO;

	*pos += count;
	pr_info("%s: %s %lu %lld\n", DEVICE_NAME, "COPIED/FPOS", count, *pos);
	return count;
}

static int __init init_function(void)
{

	pr_info("%s: %s\n", DEVICE_NAME, "INIT");
	proc_dir = proc_mkdir(PROC_NAME, NULL);
	if (IS_ERR(proc_dir)) {
		pr_err("%s: %s\n", DEVICE_NAME, "Failed to create proc_fs dir");
		goto proc_fs_dir__err;
	}
	proc_file = proc_create(PROC_NAME, PERMS, proc_dir, &pops);
	if (IS_ERR(proc_file)) {
		pr_err("%s: %s\n", DEVICE_NAME, "Failed to create proc_fs file");
		goto proc_fs_file__err;
	}
	pr_info("%s: %s\n", DEVICE_NAME, "Success");
	return 0;
	

proc_fs_file__err:
	proc_remove(proc_file);
proc_fs_dir__err:
	proc_remove(proc_dir);
	return -1;

}

static void __exit exit_function(void)
{
	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_info("%s: %s\n", DEVICE_NAME, "EXIT");
}


module_init(init_function);
module_exit(exit_function);

MODULE_AUTHOR("Yurii Klysko");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simle kernel module.");
