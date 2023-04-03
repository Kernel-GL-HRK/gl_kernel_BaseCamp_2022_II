#include <linux/init.h>         // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>       // Core header for loading LKMs into the kernel
#include <linux/kernel.h>       // Contains types, macros, functions for the kernel
#include <linux/moduleparam.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/delay.h>		//usleep(), msleep()
#include <linux/jiffies.h>


MODULE_LICENSE("GPL");                  ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Yevhen Kolesnyk");       ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple device driver - Kernel Timer with GPIO");  ///< The description -- see modinfo
MODULE_VERSION("0.1");                  ///< The version of the module

#define TIMEOUT         1000 //milliseconds
#define PROC_BUFFER_SIZE 100
#define GPIO_4 (4) //LED is connected to this GPIO

static int led_state; // 0 - disabled, 1 - enabled

static struct timer_list  etx_timer;
static unsigned int count_timer;

static char procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t procfs_buffer_size;

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

#define PROC_FILE_NAME "LED_blink"
#define PROC_DIR_NAME "GPIO_4"


static ssize_t gpio_read(struct file *File, char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (*offset >= procfs_buffer_size)
		return 0;

	to_copy = min(count, procfs_buffer_size - (size_t)*offset);

	not_copied = copy_to_user(buffer, procfs_buffer_size + *offset, to_copy);

	delta = to_copy - not_copied;
	*offset += delta;

	return delta;
}



static struct proc_ops fops = {
	.proc_read = gpio_read,

};


void timer_callback(struct timer_list *data)
{
	pr_info("TIMER: %s() %d times\n", __func__, count_timer++);

	led_state = !led_state;
	gpio_set_value(GPIO_4, led_state);

	// Format procfs_buffer with LED state and blink period
	procfs_buffer_size = sprintf(procfs_buffer, "LED state: %d, blink period: %d ms\n", led_state, TIMEOUT);

	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}


static int __init gpio_driver_init(void)
{
	pr_info("DRIVER: driver start!\n");

	count_timer = 0;

	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_folder) {
		pr_err("DRIVER: Error: Could not create /proc/%s/ folder\n", PROC_DIR_NAME);
		return -ENOMEM;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_folder, &fops);
	if (!proc_file) {
		pr_err("DRIVER: Error: Could not initialize /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		proc_remove(proc_folder);
		return -ENOMEM;
	}

	if (gpio_is_valid(GPIO_4) == false) {
		pr_err("GPIO %d is not valid\n", GPIO_4);
		goto exit;
	}

	if (gpio_request(GPIO_4, "GPIO_4") < 0) {
		pr_err("ERROR: GPIO %d request\n", GPIO_4);
		goto exit;
	}

	gpio_direction_output(GPIO_4, 0);

	pr_info("Device Driver Insert...");

	gpio_set_value(GPIO_4, 1);
	led_state = 1;

	// setup your timer to call my timer callback
	timer_setup(&etx_timer, timer_callback, 0);

	// setup timer interval to based on TIMEOUT Macro
	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));

	pr_info("TIMER: %s()\n", __func__);

	return 0;

exit:
	return -ENODEV;
}

static void __exit gpio_driver_exit(void)
{
	del_timer(&etx_timer);
	pr_info("TIMER: %s()\n", __func__);

	gpio_set_value(GPIO_4, 0);
	led_state = 0;
	gpio_free(GPIO_4);
	pr_info("Device Driver Remove... Done!!\n");

	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("DRIVER: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);

}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);
