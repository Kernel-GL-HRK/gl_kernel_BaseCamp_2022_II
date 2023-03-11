// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
#include "pr_fmt.h"
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include "proc.h"
#include "gpio.h"

static struct proc_dir_entry *proc_timeout;
static struct proc_dir_entry *proc_gpio_status;
static struct proc_dir_entry *proc_folder;

static const size_t PROC_BUFFER_CAPACITY = 8;
static char procfs_buffer[PROC_BUFFER_CAPACITY];

/*===============================================================================================*/
static ssize_t gpio_status_read(struct file *file, char __user *buffer, size_t count, loff_t *offset)
{
	if (*offset > 0)
		return 0;

	ssize_t to_copy, not_copied, buffer_size;

	buffer_size = sprintf(procfs_buffer, "%d\n", gpio_get_status());

	if (buffer_size < 0)
		return -EFAULT

	to_copy = min_t(size_t, count, buffer_size);
	not_copied = copy_to_user(buffer, procfs_buffer, to_copy);

	*offset += to_copy - not_copied;

	return to_copy - not_copied;
}
/*===============================================================================================*/
static ssize_t gpio_status_write(struct file *file, const char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied;

	to_copy = 1;
	not_copied = copy_from_user(procfs_buffer, buffer, to_copy);

	switch (procfs_buffer[0]) {
		case '1': gpio_blink_on(); break;
		case '0': gpio_blink_off(); break;
	}

	return to_copy - not_copied;
}
/*===============================================================================================*/
static struct proc_ops gpio_status_ops = {
	.proc_read	= gpio_status_read,
	.proc_write	= gpio_status_write
};
/*===============================================================================================*/
static ssize_t timeout_read(struct file *file, char __user *buffer, size_t count, loff_t *offset)
{
	if (*offset > 0)
		return 0;

	ssize_t to_copy, not_copied, buffer_size;

	buffer_size = sprintf(procfs_buffer, "%d\n", TIMEOUT);

	if (buffer_size < 0)
		return -EFAULT;

	to_copy = min(size_t, count, buffer_size);
	not_copied = copy_to_user(buffer, procfs_buffer, to_copy);

	*offset += to_copy - not_copied;

	return to_copy - not_copied;
}
/*===============================================================================================*/
static struct proc_ops timeout_ops = {
	.proc_read = timeout_read
};
/*===============================================================================================*/
int proc_init(void)
{
	proc_folder = proc_mkdir(PROC_DIR, NULL);
	if (!proc_folder) {
		pr_err("Could not create /proc/%s/ folder\n", PROC_DIR);
		return -ENOMEM;
	}

	proc_gpio_status = proc_create(PROC_GPIO_STATUS, 0666, proc_folder, &gpio_status_ops);
	if (!proc_file) {
		pr_err("Could not initialize /proc/%s/%s\n", PROC_DIR, PROC_GPIO_STATUS);
		proc_exit();
		return -ENOMEM;
	}

	proc_timeout = proc_create(PROC_TIMEOUT, 0666, proc_folder, &timeout_ops);
	if (!proc_file) {
		pr_err("Could not initialize /proc/%s/%s\n", PROC_DIR, PROC_TIMEOUT);
		proc_exit();
		return -ENOMEM;
	}

	return 0;
}
/*===============================================================================================*/
void proc_exit(void)
{
	proc_remove(proc_timeout);
	proc_remove(proc_gpio_status);
	proc_remove(proc_folder);
}
/*===============================================================================================*/
