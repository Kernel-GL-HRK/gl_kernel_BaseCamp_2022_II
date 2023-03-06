#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/gpio.h>

#define DEVICE_NAME "mymod"
#define PROC_NAME "mymod"
#define PERMS 0644
#define BUFS 6000
static char modbuf[BUFS] = {[0 ... 3000] = '1', [3001 ... BUFS-1] = '2'};
static struct proc_dir_entry *proc_dir, *proc_file;
static ssize_t mymod_read(struct file *filp, char __user *buf, size_t count, loff_t *pos);

#define PERIOD 5000
static struct timer_list my_timer;

#define LED 26

void my_timer_callback(struct timer_list * this_timer)
{
	static int level;
	level = !level;
	gpio_set_value(LED, level);
	pr_info("%s: %s %d\n", DEVICE_NAME, "SET LEVEL:", level);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(PERIOD));
}

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
	pr_info("%s: %s %u %lld\n", DEVICE_NAME, "COPIED/FPOS", count, *pos);
	return count;
}

static int __init init_function(void)
{

	pr_info("%s: %s\n", DEVICE_NAME, "INIT");
	proc_dir = proc_mkdir(PROC_NAME, NULL);
	if (IS_ERR(proc_dir))
		goto init_err;
	proc_file = proc_create(PROC_NAME, PERMS, proc_dir, &pops);
	if (IS_ERR(proc_file)) {
		proc_remove(proc_file);
		goto init_err;
	}

	if (!gpio_is_valid(LED))
		goto gpio_err;
	gpio_free(LED);

	if (!gpio_request_one(LED, GPIOF_INIT_LOW, "LED"))
		gpio_export(LED, 0);
	else
		goto gpio_err;

        timer_setup(&my_timer, my_timer_callback, 0);
	if (mod_timer(&my_timer, jiffies + msecs_to_jiffies(PERIOD))) {
		pr_info("%s: %s\n", DEVICE_NAME, "Timer firing failed");
		return -EPERM;
	}

	return 0;
	init_err:
		proc_remove(proc_dir);
		pr_err("%s: %s\n", DEVICE_NAME, "ENOMEM");
		return -ENOMEM;
	gpio_err:
		pr_err("%s: %s\n", DEVICE_NAME, "Invalid GPIO");
		return -ENODEV;
}

static void __exit exit_function(void)
{
	del_timer(&my_timer);
	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_info("%s: %s\n", DEVICE_NAME, "EXIT");
}


module_init(init_function);
module_exit(exit_function);

MODULE_AUTHOR("Yurii Klysko");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simle kernel module.");
