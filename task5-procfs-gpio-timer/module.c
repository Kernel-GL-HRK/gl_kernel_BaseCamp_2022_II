// SPDX-License-Identifier: GPL
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/err.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kushnir Vladyslav");
MODULE_DESCRIPTION("Procfs, GPIO, timer...");
MODULE_VERSION("0.1");

#define GPIO_21 (21)

#define PROC_BUFFER_SIZE 100

static char proc_buffer[PROC_BUFFER_SIZE];

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

#define PROC_FILE_NAME "my_proc_mod_file"
#define PROC_DIR_NAME "my_proc_mod_dir"

#define TIMEOUT (1000 / 20)
static struct timer_list etx_timer;
static unsigned int count;
static bool status;

static bool is_read;

static ssize_t proc_read(struct file *FIle, char __user *buffer, size_t Count, loff_t *offset)
{
	is_read = !is_read;
	if (!is_read)
		return 0;

	size_t size_to_copy  = sprintf(proc_buffer, "LED status: %d.\nPeriod time: %d ms.\nNumber of switches: %d.\n", status, TIMEOUT, count);
	size_t not_copy = copy_to_user(buffer, proc_buffer, size_to_copy);

	return size_to_copy - not_copy;
}

static ssize_t proc_write(struct file *FIle, const char __user *buffer, size_t count, loff_t *offset)
{
	return count;
}

static struct proc_ops fops = {
	.proc_read = proc_read,
	.proc_write = proc_write
};

static void timer_callback(struct timer_list *data)
{
	count++;
	status = !status;

	gpio_set_value(GPIO_21, status);

	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}

static int __init init(void)
{
	pr_info("The 'Procfs, GPIO, timer...' module starts initalization.");

	if (!gpio_is_valid(GPIO_21)) {
		pr_err("ERROR: GPIO %d is not valid\n", GPIO_21);
		goto exit;
	}

	if (gpio_request(GPIO_21, "GPIO_21") < 0) {
		pr_err("ERROR: GPIO %d request\n", GPIO_21);
		goto exit;
	}

	gpio_direction_output(GPIO_21, 0);

	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_folder) {
		pr_err("ERROR: Could not create /proc/%s/ folder\n", PROC_DIR_NAME);
		goto gpio_exit;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_folder, &fops);
	if (!proc_file) {
		pr_err("ERROR: Could not initialize /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		goto proc_exit;
	}

	timer_setup(&etx_timer, timer_callback, 0);

	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));

	pr_info("The initalization of module 'Procfs, GPIO, timer...' is successeful.");
	return 0;

proc_exit:
	proc_remove(proc_file);
	proc_remove(proc_folder);

gpio_exit:
	gpio_free(GPIO_21);

exit:
	return -1;
}

static void __exit exit_module(void)
{
	del_timer(&etx_timer);

	proc_remove(proc_file);
	proc_remove(proc_folder);

	gpio_set_value(GPIO_21, 0);

	gpio_free(GPIO_21);

	pr_info("The work of the 'first_module' module is completed.\n");
}

module_init(init);
module_exit(exit_module);
