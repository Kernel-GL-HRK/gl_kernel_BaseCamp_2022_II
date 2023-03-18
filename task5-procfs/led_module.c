// SPDX-License-Identifier: GPL

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Viacheslav Us");
MODULE_DESCRIPTION("GPIO driver with proc_fs");
MODULE_VERSION("0.1");

#define PROC_BUFFER_SIZE	100
#define PROC_FILE_NAME		"gpio"
#define PROC_DIR_NAME		"led_driver"
#define TIMEOUT			500
#define RED_LED			16

static char	procfs_buffer[PROC_BUFFER_SIZE];
static size_t	procfs_buffer_size = PROC_BUFFER_SIZE;

static struct	proc_dir_entry *proc_file;
static struct	proc_dir_entry *proc_dir;

static struct timer_list etx_timer;

void timer_callback(struct timer_list *data)
{
	gpio_set_value(RED_LED, gpio_get_value(RED_LED) ^ 1);

	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));

	procfs_buffer_size = sprintf(procfs_buffer, "Led = %d time = %d\n",
				     gpio_get_value(RED_LED), TIMEOUT);
}

static ssize_t proc_read(struct file *File, char __user *buffer,
			  size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (!procfs_buffer_size) {
		return 0;
	}

	to_copy = min(count, procfs_buffer_size);
	not_copied = copy_to_user(buffer, procfs_buffer, to_copy);

	delta = to_copy - not_copied;
	procfs_buffer_size -= delta;

	return delta;
}

static const struct proc_ops fops = {
	.proc_read = proc_read,
};

static int __init led_init(void)
{
	proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_dir) {
		pr_err("Failed to create /proc/%s folder\n",
		       PROC_DIR_NAME);
		return -ENOMEM;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_dir, &fops);
	if (!proc_file) {
		pr_err("Failed to create /proc/%s/%s\n file\n",
			PROC_DIR_NAME, PROC_FILE_NAME);
		proc_remove(proc_dir);
		return -ENOMEM;
	}

	pr_info("Created: /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);

	if (gpio_is_valid(RED_LED) == false) {
		pr_err("GPIO %d is not valid\n", RED_LED);
		proc_remove(proc_file);
		proc_remove(proc_dir);

		return -EPERM;
	}

	if (gpio_request(RED_LED, "RED_LED") < 0) {
		pr_err("GPIO %d request failed\n", RED_LED);
		proc_remove(proc_file);
		proc_remove(proc_dir);

		return -EPERM;
	}

	gpio_direction_output(RED_LED, 0);

	timer_setup(&etx_timer, timer_callback, 0);

	add_timer(&etx_timer);

	gpio_export(RED_LED, false);

	pr_info("Initialization on GPIO %d successful\n", RED_LED);

	return 0;
}

static void __exit led_exit(void)
{
	pr_info("Exiting kernel module\n");

	del_timer(&etx_timer);

	gpio_set_value(RED_LED, 0);
	gpio_unexport(RED_LED);
	gpio_free(RED_LED);

	pr_info("GPIO: Device Driver removed\n");

	proc_remove(proc_file);
	proc_remove(proc_dir);

	pr_info("PROC: /proc/%s/%s removed\n",
		PROC_DIR_NAME, PROC_FILE_NAME);

}

module_init(led_init);
module_exit(led_exit);
