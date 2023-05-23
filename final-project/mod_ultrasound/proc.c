#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/printk.h>
#include "proc.h"
#include "platform_driver.h"

ssize_t proc_read (struct file *filep, char *to_user, size_t len, loff_t *offs);

struct proc_dir_entry *proc_file;
struct proc_ops proc_fops = {
	.proc_read = proc_read,
};

MODULE_LICENSE("GPL");

int32_t create_procFS(void)
{
	proc_file = proc_create(PROC_FILE_NAME, ONLY_READ_MODE, NULL, &proc_fops);
	if(proc_file == NULL) {
		pr_err("ultrasound-proc: can not create file proc: %s\n", PROC_FILE_NAME);
		return -ENOMEM;
	}
	pr_info("ultrasound-proc: file proc %s, is created\n", PROC_FILE_NAME);
	return 0;
}

void remove_procFS(void)
{
	proc_remove(proc_file);
	pr_info("ultrasound-proc: file proc %s, is removed\n", PROC_FILE_NAME);
}

ssize_t proc_read (struct file *filep, char *to_user, size_t len, loff_t *offs)
{
	size_t to_copy;
	int8_t buffer_for_copy[PROC_MAX_BUFFER_SIZE] = {0};
	struct ultrasound_desc gpio_of_ultrasound;

	if (PROC_MAX_BUFFER_SIZE > len) {
		pr_err("ultrasound-proc-read: too little space of mem to output text to file\n");
		return -ENOMEM;
	}
	gpio_of_ultrasound = get_ultra_description();
	switch (*offs) {
	case FIRST_MESSAGE:
		to_copy = snprintf(buffer_for_copy, PROC_MAX_BUFFER_SIZE, INTRODUCTION);
		goto ready_for_sending;
	case US_TRIG:
		if (!strcmp(gpio_of_ultrasound.status, "disconnected"))
			to_copy = snprintf(buffer_for_copy, PROC_MAX_BUFFER_SIZE, "ECHO PIN = [undefined]\n");
		else
			to_copy = snprintf(buffer_for_copy, PROC_MAX_BUFFER_SIZE, "ECHO PIN = [%d]\n", gpio_of_ultrasound.echo_pin);

		goto ready_for_sending;
	case US_ECHO:
		if (!strcmp(gpio_of_ultrasound.status, "disconnected"))
			to_copy = snprintf(buffer_for_copy, PROC_MAX_BUFFER_SIZE, "TRIGGER PIN = [undefined]\n");
		else
			to_copy = snprintf(buffer_for_copy, PROC_MAX_BUFFER_SIZE, "TRIGGER PIN = [%d]\n", gpio_of_ultrasound.trig_pin);

		goto ready_for_sending;
	}
	return 0;
ready_for_sending:
	if (copy_to_user(to_user, buffer_for_copy, to_copy)) {
		pr_err("ultrasound-proc-read: can not copy to user space\n");
		return -ENOMEM;
	}
	*offs = *offs + 1;
	return to_copy;
}