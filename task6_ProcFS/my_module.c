// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
/*
 * Kernel module
 * During init it prints info message "Hello..."
 * Calculates sum, subtraction and product of two parameters
 * /proc fs implemented
 * During exit it prints info message "Goodbye..."
 * Blinking GPIO with timer
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <../arch/powerpc/boot/stdio.h>
//#include <linux/string.h>
//#include <linux/err.h>
//#include <uapi/linux/string.h>
//#include <linux/delay.h>	//usleep(), msleep()
//#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vadym Minziuk");
MODULE_DESCRIPTION("Hello from Kernel Module with ProcFS and GPIO driver with timer");
MODULE_VERSION("0.4");

#define PROC_BUFFER_SIZE 100
#define PROC_FILE_NAME "status"
#define PROC_DIR_NAME "my_module"
#define GPIO_4 (4) //LED is connected to this GPIO
#define TIMEOUT 5000 //milliseconds

static int num1;
module_param(num1, int, 0664);

static int num2;
module_param(num2, int, 0664);

static unsigned int led_state;

static char   procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t procfs_buffer_size;

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

static ssize_t hello_read(struct file *File, char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, have_copied;

	if (procfs_buffer_size == 0)
		return 0;

	//pr_info("HELLO: User spase want to copy = %d\n", count);
	to_copy = min_t(size_t, count, procfs_buffer_size);
	//pr_info("HELLO: to_copy = %d\n", to_copy);

	not_copied = copy_to_user(buffer, procfs_buffer, to_copy);
	//pr_info("HELLO: not_copied = %d\n", not_copied);

	have_copied = to_copy - not_copied;
	procfs_buffer_size = procfs_buffer_size - have_copied;

	return have_copied;
}

static ssize_t hello_write(struct file *file, const char __user *buffer, size_t count, loff_t *offset)
{
/*
 * This function does not have offset handler,
 * so it can transfer only one page of data (1024 B)
 */
	ssize_t to_copy, not_copied, delta;

	to_copy = min_t(size_t, count, PROC_BUFFER_SIZE);

	not_copied = copy_from_user(procfs_buffer, buffer, to_copy);

	delta = to_copy - not_copied;

	procfs_buffer_size = delta;

	return delta;
}

static struct proc_ops fops = {
	.proc_read  = hello_read,
	.proc_write = hello_write
};
//static struct file_operations fops = {
//	.owner = THIS_MODULE,
//	.read  = hello_read,
//	.write = hello_write
//};

//Timer
static struct timer_list	etx_timer;
static unsigned int count;

void timer_callback(struct timer_list *data)
{
	pr_info("TIMER: %s(), %d times\n", __func__, count++);

	if (led_state)
		led_state = 0;
	else
		led_state = 1;

	gpio_set_value(GPIO_4, led_state);

	sprintf(procfs_buffer, "LED state is %d\nPeriod = %d ms\n", led_state, TIMEOUT * 2);
	procfs_buffer_size = sizeof(procfs_buffer);

	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}

static int __init hello_init(void)
{
	int err_exit = 0;

	pr_info("HELLO: Hello from Kernel Module!\n");
	if (num1 || num2) {
		pr_info("HELLO: sum = %d\n", 0 + num1 + num2);
		pr_info("HELLO: subtraction = %d\n", 0 + num1 - num2);
		pr_info("HELLO: product = %d\n", 0 + num1 * num2);
	}

	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_folder) {
		pr_err("HELLO: Error: Could not initialize /proc/%s folder\n", PROC_DIR_NAME);
		err_exit = -1;
		goto err_exit_1;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_folder, &fops);
	if (!proc_file) {
		pr_err("HELLO: Error: Could not initialize file /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		err_exit = -2;
		goto err_exit_2;
	}

	pr_info("HELLO: /proc/%s/%s created\n", PROC_DIR_NAME, PROC_FILE_NAME);

	if (gpio_is_valid(GPIO_4) == false) {
		pr_err("ERROR: GPIO %d is not valid\n", GPIO_4);
		err_exit = -3;
		goto err_exit_3;
	}

	if (gpio_request(GPIO_4, "GPIO_4") < 0) {
		pr_err("ERROR: GPIO %d request\n", GPIO_4);
		err_exit = -4;
		goto err_exit_4;
	}

	gpio_direction_output(GPIO_4, 0);

	/* Using this call the GPIO 4 will be visible in /sys/class/gpio/
	 * Now you can change the gpio value by using below commands also.
	 * echo 1 > /sys/class/gpio/gpio4/value (turn ON the LED)
	 * echo 0 > /sys/class/gpio/gpio4/value (turn OFF the LED)
	 * cat /sys/class/gpio/gpio4/value (read the value LED)
	 *
	 * The second argument prevents the direction from being changed
	 */

	gpio_export(GPIO_4, false);

	gpio_set_value(GPIO_4, led_state);

	// Setup your timer to call my_timer_callback
	timer_setup(&etx_timer, timer_callback, 0);

	// Setup timer interval to based on TIMEOUT Macro
	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
	pr_info("TIMER: %s()\n", __func__);

	pr_info("Device Driver Insert... Done!!!\n");
	err_exit = 0;
	goto err_exit_0;

//err_exit_5:
//	gpio_free(GPIO_4);

err_exit_4:
err_exit_3:
	proc_remove(proc_file);

err_exit_2:
	proc_remove(proc_folder);

err_exit_1:
err_exit_0:
	return err_exit;
}

static void __exit hello_exit(void)
{
	del_timer(&etx_timer);
	pr_info("TIMER: %s()\n", __func__);

	gpio_set_value(GPIO_4, 0);
	gpio_unexport(GPIO_4);
	gpio_free(GPIO_4);

	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("HELLO: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
	pr_info("HELLO: Device Driver Remove... Done!!!\n");
}

module_init(hello_init);
module_exit(hello_exit);
