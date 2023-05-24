// SPDX-License-Identifier: GPL

#define DEBUG

#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/gpio.h>

#define DEFAULT_GPIO_PIN	26
#define DEFAULT_ON_DURATION	500
#define DEFAULT_OFF_DURATION	500
#define DEFAULT_LED_STATE	0

#define PROC_BUFFER_SIZE	1024
#define PROC_DIR_NAME		"blinker"
#define PROC_FILE_NAME		"gpio"

static int gpio = DEFAULT_GPIO_PIN;
module_param(gpio, int, 0444);
MODULE_PARM_DESC(gpio, "GPIO Pin");

static int on = DEFAULT_ON_DURATION;
module_param(on, int, 0444);
MODULE_PARM_DESC(on, "LED ON duration in milliseconds");

static int off = DEFAULT_OFF_DURATION;
module_param(off, int, 0444);
MODULE_PARM_DESC(off, "LED OFF duration in milliseconds");

static char procfs_buffer[PROC_BUFFER_SIZE];
static size_t procfs_bufsize;
static int led_state = DEFAULT_LED_STATE;

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;
static struct timer_list timer;

static unsigned int timeout;
static unsigned int switches;

static ssize_t proc_read(struct file *fp,
			 char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t len_to_copy;

	if (*offset >= procfs_bufsize)
		return 0;

	len_to_copy = min_t(int, count, procfs_bufsize - *offset);

	if (len_to_copy <= 0)
		return 0;

	if (copy_to_user(buffer, procfs_buffer + *offset, len_to_copy))
		return -EFAULT;

	*offset += len_to_copy;

	return len_to_copy;
}

static const struct proc_ops proc_fops = {
	.proc_read = proc_read,
};

static inline void update_procfs_buffer(void)
{
	procfs_bufsize = sprintf(procfs_buffer,
		"GPIO Pin\t: %d\n"
		"On Duration\t: %d\n"
		"Off Duration\t: %d\n\n"
		"LED Status\t: %s\n"
		"Current timeout\t: %d\n"
		"Switches\t: %d\n",
		gpio, on, off, led_state ? "On" : "Off", timeout, switches);
}

static void timer_callback(struct timer_list *data)
{
	timeout = led_state ? off : on;
	switches += 1;
	led_state ^= 1;

	gpio_set_value(gpio, led_state);
	mod_timer(&timer, jiffies + msecs_to_jiffies(timeout));

	update_procfs_buffer();
}

static int __init blinker_init(void)
{
	char proc_file_name[NAME_MAX];

	pr_notice("Initializing kernel module...\n");

	proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_dir) {
		pr_err("Failed to create proc dir /proc/%s\n", PROC_DIR_NAME);
		return -ENOMEM;
	}

	sprintf(proc_file_name, PROC_FILE_NAME "%d", gpio);
	proc_file = proc_create(proc_file_name, 0444, proc_dir, &proc_fops);
	if (!proc_file) {
		pr_err("Failed to create proc file %s\n", proc_file_name);
		proc_remove(proc_dir);
		return -ENOMEM;
	}

	if (gpio_is_valid(gpio) == false) {
		pr_err("GPIO %d is not valid\n", gpio);
		proc_remove(proc_file);
		proc_remove(proc_dir);
		return -EPERM;
	}

	if (gpio_request(gpio, proc_file_name) < 0) {
		pr_err("ERROR: GPIO %d request failed\n", gpio);
		proc_remove(proc_file);
		proc_remove(proc_dir);
		return -EPERM;
	}

	gpio_direction_output(gpio, 0);
	timer_setup(&timer, timer_callback, 0);
	add_timer(&timer);

	pr_notice("Initialization on GPIO %d successful!\n", gpio);

	return 0;
}

static void __exit blinker_exit(void)
{
	pr_notice("Exiting kernel module.\n");

	gpio_set_value(gpio, DEFAULT_LED_STATE);

	del_timer(&timer);
	gpio_free(gpio);
	proc_remove(proc_file);
	proc_remove(proc_dir);
}

module_init(blinker_init);
module_exit(blinker_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vlad Degtyarov <deesyync@gmail.com>");
MODULE_DESCRIPTION("Simple LED blinker driver");

#undef DEBUG
