#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/err.h>
#include <linux/printk.h>
#include <linux/string.h>
#include "proc.h"
#include "servo_ctl.h"
#include "platform_driver.h"

struct proc_dir_entry *proc_file;
struct proc_ops proc_fops = {
	.proc_read = proc_read,
};

MODULE_LICENSE("GPL");

int32_t create_procFS(void)
{
	proc_file = proc_create(PROC_FILE_NAME, ONLY_READ_MODE, NULL, &proc_fops);
	if(proc_file == NULL) {
		pr_err("servo-proc: can not create file proc: %s\n", PROC_FILE_NAME);
		return -ENOMEM;
	}
	pr_info("servo-proc: file proc %s, is created\n", PROC_FILE_NAME);
	return 0;
}

void remove_procFS(void)
{
	proc_remove(proc_file);
	pr_info("servo-proc: file proc %s, is removed\n", PROC_FILE_NAME);
}

ssize_t proc_read (struct file *filep, char *to_user, size_t len, loff_t *offs)
{
	size_t to_copy;
	int8_t buffer_for_copy[PROC_MAX_BUFFER_SIZE] = {0};
	struct servo_params servo_desc;

	if(PROC_MAX_BUFFER_SIZE > len) {
		pr_err("servo-proc-read: too little space of mem to output text to file\n");
		return -ENOMEM;
	}
	servo_desc = get_servo_description();
	switch (*offs) {
	case FIRST_MESSAGE:
		to_copy = snprintf(buffer_for_copy, PROC_MAX_BUFFER_SIZE, INTRODUCTION);
		goto ready_for_sending;
	case SERVO_CHANNEL:
		if(!strcmp(servo_desc.status, "disabled"))
			to_copy = snprintf(buffer_for_copy, PROC_MAX_BUFFER_SIZE , "PWM CHANNEL = [undefined]\n");
		else
			to_copy = snprintf(buffer_for_copy, PROC_MAX_BUFFER_SIZE , "PWM CHANNEL = [%d]\n", servo_desc.pwm_channel);

		goto ready_for_sending;
	case SERVO_MODE:
		if(!strcmp(servo_desc.status, "disabled"))
			to_copy = snprintf(buffer_for_copy, PROC_MAX_BUFFER_SIZE , "SERVO MODE = [undefined]\n");
		else
			to_copy = snprintf(buffer_for_copy, PROC_MAX_BUFFER_SIZE , "SERVO MODE = [%s]\n", servo_desc.mode);

		goto ready_for_sending;
	}
	return 0;
ready_for_sending:
	if (copy_to_user(to_user, buffer_for_copy, to_copy)) {
		pr_err("servo-proc-read: can not copy to user space\n");
		return -ENOMEM;
	}
	*offs = *offs + 1; 
	return to_copy;
}