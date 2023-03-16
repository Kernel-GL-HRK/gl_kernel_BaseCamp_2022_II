// SPDX-License-Identifier: GPL
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergiy Us");
MODULE_DESCRIPTION("A simple device driver - procfs module");
MODULE_VERSION("0.1");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define	HAVE_PROC_OPS
#endif

#define PROC_BUFFER_SIZE	100
#define PROC_DIR_NAME		"hello"		/* /proc/hello/dummy */
#define PROC_FILE_NAME		"dummy"
#define TIMEOUT			1000		/* milliseconds */

static char   procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t procfs_buffer_size;
static unsigned int timer_count;

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_dir;
static struct timer_list etx_timer;

static ssize_t hello_read(struct file *File, char __user *buffer,
			  size_t count, loff_t *offset);
static ssize_t hello_write(struct file *file, const char __user *buffer,
			   size_t count, loff_t *offset);
static inline int proc_init(void);
static void timer_callback(struct timer_list *data);
static inline void timer_init(void);

#ifdef HAVE_PROC_OPS
static const struct proc_ops fops = {
	.proc_read  = hello_read,
	.proc_write = hello_write,
};
#else
static const struct file_operations fops = {
	.read  = hello_read,
	.write = hello_write,
};
#endif

static int __init hello_init(void)
{
	int ret;

	ret = proc_init();
	timer_init();

	return ret;
}

static void __exit hello_exit(void)
{
	del_timer(&etx_timer);
	pr_info("HELLO: TIMER: %s()\n", __func__);

	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_info("HELLO: PROC: /proc/%s/%s removed\n",
		PROC_DIR_NAME, PROC_FILE_NAME);
}

module_init(hello_init);
module_exit(hello_exit);

static ssize_t hello_read(struct file *File, char __user *buffer,
			  size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (!procfs_buffer_size)
		return 0;

	to_copy = min(count, procfs_buffer_size);

	not_copied = copy_to_user(buffer, procfs_buffer, to_copy);

	delta = to_copy - not_copied;

	pr_debug("DEBUG: PROC: count(read) = %d\n", count);
	pr_debug("DEBUG: PROC: delta(read) = %d\n", delta);
	pr_debug("DEBUG: PROC: procfs_buffer_size(read) = %d\n",
		 procfs_buffer_size);

	procfs_buffer_size -= delta;

	return delta;
}

static ssize_t hello_write(struct file *file, const char __user *buffer,
			   size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	to_copy = min(count, sizeof(procfs_buffer));

	not_copied = copy_from_user(procfs_buffer, buffer, to_copy);
	delta = to_copy - not_copied;

	procfs_buffer_size = delta;

	pr_debug("DEBUG: PROC: count(write) = %d\n", count);
	pr_debug("DEBUG: PROC: procfs_buffer_size(write) = %d\n",
		 procfs_buffer_size);

	return delta;
}

static inline int proc_init(void)
{
	int ret = 0;

	proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_dir) {
		pr_err("HELLO: ERROR: Could not create /proc/%s directory\n",
		       PROC_DIR_NAME);
		ret = -ENOMEM;
		goto exit;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_dir, &fops);
	if (!proc_file) {
		pr_err("HELLO: ERROR: Could not initialize /proc/%s/%s\n",
		       PROC_DIR_NAME, PROC_FILE_NAME);
		proc_remove(proc_file);
		proc_remove(proc_dir);
		ret = -ENOMEM;
		goto exit;
	}

	pr_info("HELLO: PROC: /proc/%s/%s created\n",
		PROC_DIR_NAME, PROC_FILE_NAME);

exit:
	return ret;
}

static void timer_callback(struct timer_list *data)
{
	pr_debug("DEBUG: TIMER: %s() %d times\n", __func__, timer_count++);
	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}

static inline void timer_init(void)
{
	/* setup your timer to call my_timer_callback */
	timer_setup(&etx_timer, timer_callback, 0);

	/* setup timer interval is based on TIMEOUT Macro */
	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));

	pr_info("HELLO: TIMER: %s()\n", __func__);
}
