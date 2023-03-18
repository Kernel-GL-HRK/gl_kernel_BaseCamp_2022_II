// SPDX-License-Identifier: GPL-2.0-only
/*
 * Leds GPIO's driver for RGB leds.
 *
 * Dmytro Volkov <splissken2014@gmail.com>
 *
 */

 #include <linux/module.h>
 #include <linux/kernel.h>
 #include <linux/proc_fs.h>   //procfs
 #include <linux/uaccess.h>   //ctu &cfu
 #include <linux/errno.h>

 #include "project1.h"


 #define MODULE_NAME module_name(THIS_MODULE)
 #define DEVICE_NAME "leds_rgb"
 #define PROC_DIR_NAME "project1"


static struct proc_dir_entry *dev_class_proc_dir; //our procfs dir
static struct proc_dir_entry *device_proc_file;   //our procfs file

static uint8_t count;


/* proc_fs read */
static ssize_t gpio_leds_procfs_read(struct file *file, char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t ret;

	/* flush read buffer */
	memset(fs_buffer, 0, BUFFER_MAX_SIZE); //clear buffer from old garbage

	/* append blink data to read buffer */
	for (count = 0; count < ARRAY_SIZE(leds_gpios); count++)
		fs_buffer_size = snprintf(fs_buffer, BUFFER_MAX_SIZE, "%s\n%s State (1/0)):%d Timeout(ms): %d\n", fs_buffer, leds_gpios[count].label, blink.led_num[count], blink.blink_ms[count]);

	if (*offset >= fs_buffer_size) {
		/* we have finished to read, return 0 */

		ret  = 0;
	} else {
		/* fill the buffer, return the buffer size */
		copy_to_user(buffer, fs_buffer, fs_buffer_size);
		*offset += fs_buffer_size;
		ret = fs_buffer_size;
	}
	return ret;
}


static struct proc_ops gpio_leds_procfs_ops = {
	.proc_read = gpio_leds_procfs_read,
};

int leds_rgb_enable_procfs(void)
{

		dev_class_proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!dev_class_proc_dir) {
		pr_err("%s: Cant create proc folder", MODULE_NAME);
	return -ENOMEM;
	}

	device_proc_file = proc_create(DEVICE_NAME, 0664, dev_class_proc_dir,
						&gpio_leds_procfs_ops);
	return 0;
}

void leds_rgb_exit_procfs(void)
{

	proc_remove(device_proc_file);
	proc_remove(dev_class_proc_dir);
}

