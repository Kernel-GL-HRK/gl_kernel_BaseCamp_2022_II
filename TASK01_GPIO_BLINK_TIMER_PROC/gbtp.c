// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Yurii Bezkor");
MODULE_DESCRIPTION("ProcFS feature");
MODULE_VERSION("0.3");

#define LED_DEFAULT -1
static int led_status = -1;
static int led_pin = LED_DEFAULT;
module_param(led_pin, int, 0644);
MODULE_PARM_DESC(led_pin, "Led pin number for Blink");

static unsigned int timer_count = 1000;
module_param(timer_count, int, 0644);
MODULE_PARM_DESC(timer_count, "Timer period");

static struct timer_list timer_gbtp;

void timer_callback(struct timer_list *data)
{
	pr_info("GBTP: Timer call\n");
	led_status = !led_status;
	mod_timer(&timer_gbtp, jiffies + msecs_to_jiffies(timer_count));
}

#define PROCFS_BUFFER_SIZE 100
static char procfs_buffer[PROCFS_BUFFER_SIZE] = { 0 };

static struct proc_dir_entry *procfs_timer;
static struct proc_dir_entry *procfs_gpio;
static struct proc_dir_entry *procfs_dir;

#define PROCFS_DIR_NAME "gbtp"
#define PROCFS_GPIO_NAME "led"
#define PROCFS_TIMER_NAME "timout"

static ssize_t procfs_gpio_read(struct file *file, char __user *buffer,
				size_t count_bytes, loff_t *offset)
{
	size_t buffer_size;

	pr_info("Read gpio\n");

	buffer_size = sprintf(procfs_buffer, "LED status = %d\n", led_status);

	if (*offset > 0 || count_bytes < buffer_size) {
		pr_err("Illegal buffer\n");
		return 0;
	}

	if (copy_to_user(buffer, procfs_buffer, buffer_size)) {
		pr_err("Error copy_to_user\n");
		return -EFAULT;
	};
	*offset = buffer_size;
	return buffer_size;
}

static ssize_t procfs_timer_read(struct file *file, char __user *buffer,
				 size_t count_bytes, loff_t *offset)
{
	size_t buffer_size;

	pr_info("Read timeout\n");

	buffer_size = sprintf(procfs_buffer, "Timeout = %d\n", timer_count);

	if (*offset > 0 || count_bytes < buffer_size) {
		pr_err("Illegal buffer\n");
		return 0;
	}

	if (copy_to_user(buffer, procfs_buffer, buffer_size)) {
		pr_err("Error copy_to_user\n");
		return -EFAULT;
	};
	*offset = buffer_size;
	return buffer_size;
}

static ssize_t procfs_timer_write(struct file *file, const char __user *buffer,
				  size_t count_bytes, loff_t *offset)
{
	size_t buffer_size;

	pr_info("Write timeout\n");

	if (*offset > 0 || count_bytes > PROCFS_BUFFER_SIZE) {
		pr_err("Illegal buffer\n");
		return -EFAULT;
	}

	if (copy_from_user(procfs_buffer, buffer, count_bytes)) {
		pr_err("Error copy_from_user\n");
		return -EFAULT;
	};

	buffer_size = count_bytes;
	pr_info("Buffer is %s\n", procfs_buffer);

	return buffer_size;
}

const struct proc_ops gpio_fops = { .proc_read = procfs_gpio_read };

const struct proc_ops timer_fops = { .proc_read = procfs_timer_read,
				     .proc_write = procfs_timer_write };

static int __init gbtp_init(void)
{
	pr_info("GBTP: LedPin = %d\n", led_pin);
	pr_info("GBTP: TimerC = %d\n", timer_count);

	procfs_dir = proc_mkdir(PROCFS_DIR_NAME, NULL);
	if (procfs_dir == NULL) {
		pr_err("GBTP PROC: Could not create /proc/%s/ folder !\n",
		       PROCFS_DIR_NAME);
		return -ENOMEM;
	}

	procfs_gpio =
		proc_create(PROCFS_GPIO_NAME, 0666, procfs_dir, &gpio_fops);
	if (procfs_gpio == NULL) {
		pr_err("GBTP PROC: Could not create /proc/%s/%s !\n",
		       PROCFS_DIR_NAME, PROCFS_GPIO_NAME);
		proc_remove(procfs_gpio);
		proc_remove(procfs_dir);
		return -ENOMEM;
	}

	procfs_timer =
		proc_create(PROCFS_TIMER_NAME, 0666, procfs_dir, &timer_fops);
	if (procfs_timer == NULL) {
		pr_err("GBTP PROC: Could not create /proc/%s/%s !\n",
		       PROCFS_DIR_NAME, PROCFS_TIMER_NAME);
		proc_remove(procfs_gpio);
		proc_remove(procfs_timer);
		proc_remove(procfs_dir);
		return -ENOMEM;
	}

	pr_info("GBTP PROC: Success create /proc/%s/%s !\n", PROCFS_DIR_NAME,
		PROCFS_GPIO_NAME);
	pr_info("GBTP PROC: Success create /proc/%s/%s !\n", PROCFS_DIR_NAME,
		PROCFS_TIMER_NAME);

	timer_setup(&timer_gbtp, timer_callback, 0);
	mod_timer(&timer_gbtp, jiffies + msecs_to_jiffies(timer_count));

	pr_info("GBTP: Load success!\n");
	return 0;
}
static void __exit gbtp_exit(void)
{
	del_timer(&timer_gbtp);
	proc_remove(procfs_gpio);
	proc_remove(procfs_timer);
	proc_remove(procfs_dir);

	pr_info("GBTP: Unload done!\n");
}

module_init(gbtp_init);
module_exit(gbtp_exit);
