#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/gpio.h>

#define DEVICE_NAME "mymod"
#define PROC_NAME "mymod"
/* Permissions to the procfs file */
#define PERMS 0644

/* Buffer to send to userspace */
#define BUFS 256
static char modbuf[BUFS] = {0};

/* proc_fs directory and file */
static struct proc_dir_entry *proc_dir, *proc_file;

static ssize_t mymod_read(struct file *filp, char __user *buf, size_t count, loff_t *pos);

/* Time of lightning or sleeping (in ms) */
#define PERIOD 5000
static struct timer_list my_timer;

/* GPIO, where LED is connected */
#define LED 26

/*  Recursive timer handler, switch gpio level at each entry. */
void my_timer_callback(struct timer_list *this_timer)
{
	static int level;

	level = !level;
	gpio_set_value(LED, level);
	pr_info("%s: %s %d\n", DEVICE_NAME, "SET LEVEL:", level);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(PERIOD));
}

/* proc_fc file operations */
static struct proc_ops pops = {
	.proc_read = mymod_read,
};

/* The read callback. It sends to userspace  current gpio level and duration of each level. */
static ssize_t mymod_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	/*  Forming a string to send */
	sprintf(modbuf, "Current LED level: %d\nPeriod of the signal (ms): %d\n", gpio_get_value(LED), PERIOD);

	/* steps to adjust count properly and prevent reading beyond the file */
	if (*pos >= BUFS)
		return 0;
	if (*pos + count > BUFS)
		count = BUFS - (*pos);
	if (copy_to_user(buf, modbuf + (*pos), count))
		return -EIO;

	*pos += count;
	return count;
}

/* Init function */
static int __init init_function(void)
{

	pr_info("%s: %s\n", DEVICE_NAME, "INIT");

	/* Creation of a directory and a file in /proc */
	proc_dir = proc_mkdir(PROC_NAME, NULL);
	if (IS_ERR(proc_dir))
		goto init_err;
	proc_file = proc_create(PROC_NAME, PERMS, proc_dir, &pops);
	if (IS_ERR(proc_file)) {
		proc_remove(proc_file);
		goto init_err;
	}

	/* Check whatever the GPIO is valid */
	if (!gpio_is_valid(LED))
		goto gpio_err;

	gpio_free(LED);
	/* If is freed, the next is not required */
	/* Check if gpio is not busy. */
	if (gpio_request_one(LED, GPIOF_INIT_LOW, "LED"))
		goto gpio_err;

	/* Timer setup */
	timer_setup(&my_timer, my_timer_callback, 0);
	if (mod_timer(&my_timer, jiffies + msecs_to_jiffies(PERIOD)))
		goto timer_err;

	return 0;

init_err:
	proc_remove(proc_dir);
	pr_err("%s: %s\n", DEVICE_NAME, "Procfs Error");
	return -ENOMEM;
gpio_err:
	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_err("%s: %s\n", DEVICE_NAME, "Invalid GPIO");
	return -ENODEV;
timer_err:
	gpio_free(LED);
	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_err("%s: %s\n", DEVICE_NAME, "Timer firing failed");
	return -EPERM;
}
/* Exit function */
static void __exit exit_function(void)
{
	gpio_free(LED);
	del_timer(&my_timer);
	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_info("%s: %s\n", DEVICE_NAME, "EXIT");
}


module_init(init_function);
module_exit(exit_function);

MODULE_AUTHOR("Yurii Klysko");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LED blinkong module with procfs file.");
