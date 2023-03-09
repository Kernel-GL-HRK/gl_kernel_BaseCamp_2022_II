// SPDX-License-Identifier: GPL

/*******************************************************************************
*  driver.c
*  Simple GPIO driver explanation
*******************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Laura Shcherbak <laura.shcherbak@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - GPIO Blinker Driver");
MODULE_VERSION("1.0");

#define GPIO_6 6
#define DURATION 1000

static int ledOn = DURATION;
static int ledOff = DURATION;
static int ledState = 0;

static char procfs_buffer[1024];
static size_t procfs_bufsize;

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;
static struct timer_list timer;

static unsigned int timeout;
static unsigned int count;

static ssize_t proc_read(struct file *fp, char __user *buffer, size_t count, loff_t *offset)
{
        ssize_t len_to_copy;
        if (*offset >= procfs_bufsize)
                return 0;
        len_to_copy = min_t(int, count, procfs_bufsize - *offset);
        if (len_to_copy <= 0)
                return 0;
        if (copy_to_user(buffer, procfs_buffer + *offset, len_to_copy))
                return -EFAULT;
        *offset += len_to_copy;
        return len_to_copy;
}

static const struct proc_ops proc_fops = {
        .proc_read = proc_read,
};

static void timer_callback(struct timer_list *data)
{
        timeout = ledState ? ledOff : ledOn;
        count += 1;
        ledState ^= 1;
        gpio_set_value(GPIO_6, ledState);
        mod_timer(&timer, jiffies + msecs_to_jiffies(timeout));
        procfs_bufsize = sprintf(procfs_buffer,
                "GPIO PIN\t: %d\n"
                "On Duration\t: %d\n"
                "Off Duration\t: %d\n\n"
                "LED Status\t: %s\n"
                "Current timeout\t: %d\n"
                "Count\t: %d\n",
                GPIO_6,
				ledOn,
				ledOff,
				ledState ? "On" : "Off",
				timeout,
				count);
}

static int __init blinker_init(void)
{
        pr_notice("Initializing kernel module...\n");
        proc_dir = proc_mkdir("blinker", NULL);
        if (!proc_dir) {
                pr_err("Failed to create proc dir /proc/%s\n", "blinker");
                return -ENOMEM;
        }
        proc_file = proc_create("gpio", 0444, proc_dir, &proc_fops);
        if (!proc_file) {
                pr_err("Failed to create proc file %s\n", "gpio");
                proc_remove(proc_dir);
                return -ENOMEM;
        }
        if (gpio_is_valid(GPIO_6) == false) {
                pr_err("GPIO %d is not valid\n", GPIO_6);
                proc_remove(proc_file);
                proc_remove(proc_dir);
                return -EPERM;
        }
        if (gpio_request(GPIO_6, "gpio") < 0) {
                pr_err("ERROR: GPIO %d request failed\n", GPIO_6);
                proc_remove(proc_file);
                proc_remove(proc_dir);
                return -EPERM;
        }
        gpio_direction_output(GPIO_6, 0);
        timer_setup(&timer, timer_callback, 0);
        add_timer(&timer);
        pr_notice("Initialization on GPIO %d successful!\n", GPIO_6);
        return 0;
}

static void __exit blinker_exit(void)
{
        pr_notice("Exiting kernel module.\n");
        gpio_set_value(GPIO_6, 0);
        del_timer(&timer);
        gpio_free(GPIO_6);
        proc_remove(proc_file);
        proc_remove(proc_dir);
}

module_init(blinker_init);
module_exit(blinker_exit);
