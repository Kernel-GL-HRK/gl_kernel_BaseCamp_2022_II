// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kostiantyn Makhno");
MODULE_DESCRIPTION("Simple led driver");
MODULE_VERSION("1.0");

#define LED_PIN		23UL
#define PERIOD		1000U
#define PROC_BUFF_LEN	1024UL
#define PROC_DIR_NAME	"hello"
#define PROC_FILE_NAME	"drv_info"

static char proc_buff[PROC_BUFF_LEN];

struct driver_data {
	struct timer_list tm;
	unsigned int tm_period;
	unsigned int led_state;
	size_t buff_len;
	char *buff;
};

static struct driver_data drv_data;
static struct proc_dir_entry *pdir;
static struct proc_dir_entry *pfile;

ssize_t
hello_proc_read(struct file *filp, char __user *ubuf, size_t count, loff_t *offset)
{
	size_t to_copy;

	to_copy = min(count, drv_data.buff_len);

	sprintf(drv_data.buff, "led_state: %u\nperiod: %u\n",
		drv_data.led_state, drv_data.tm_period);

	if (copy_to_user(ubuf, drv_data.buff, to_copy))
		return -EFAULT;

	return to_copy;
}

const struct proc_ops proc_fops = {
	.proc_read = hello_proc_read
};

void tm_callback(struct timer_list *ptm)
{
	struct driver_data *drv;

	drv = container_of(ptm, struct driver_data, tm);
	drv->led_state = (~drv->led_state) & 0x1;
	gpio_set_value(LED_PIN, drv->led_state);
	mod_timer(&drv->tm, jiffies + msecs_to_jiffies(drv->tm_period));
}

static int __init hello_init(void)
{
	if (!gpio_is_valid(LED_PIN)) {
		pr_info("invalid gpio pin %ld\n", LED_PIN);
		goto err_valid_gpio;
	}

	if (gpio_request(LED_PIN, "led_pin") < 0) {
		pr_info("can not request %ld pin", LED_PIN);
		goto err_req_gpio;
	}

	if (gpio_direction_output(LED_PIN, drv_data.led_state) != 0) {
		pr_info("can not set direction on %ld pin\n", LED_PIN);
		goto err_set_dir_gpio;
	}

	pdir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (pdir == NULL) {
		pr_err("can not create directory /proc/%s\n", PROC_DIR_NAME);
		return -ENOMEM;
	}

	pfile = proc_create(PROC_FILE_NAME, 0666, pdir, &proc_fops);
	if (pfile == NULL) {
		proc_remove(pdir);
		pr_err("can not create /proc/%s/%s file\n", PROC_DIR_NAME, PROC_FILE_NAME);
		return -ENOMEM;
	}
	drv_data.tm_period = PERIOD;
	drv_data.buff      = proc_buff;
	drv_data.buff_len  = PROC_BUFF_LEN;
	timer_setup(&drv_data.tm, tm_callback, 0);
	mod_timer(&drv_data.tm, jiffies + msecs_to_jiffies(drv_data.tm_period));

	return 0;

err_set_dir_gpio:
err_req_gpio:
	gpio_free(LED_PIN);
err_valid_gpio:
	return -EINVAL;
}

static void __exit hello_exit(void)
{
	del_timer(&drv_data.tm);
	proc_remove(pfile);
	proc_remove(pdir);
	gpio_set_value(LED_PIN, 0);
	gpio_free(LED_PIN);
	pr_info("led pin %ld was freed\n", LED_PIN);
}

module_init(hello_init);
module_exit(hello_exit);
