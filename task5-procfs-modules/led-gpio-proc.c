#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/proc_fs.h>

#define LED 14
#define PROC_FILENAME "led_blink"

static struct timer_list blink_timer;
static int led_status = 0;
static int blink_period_ms = 500;

static ssize_t led_proc_read(struct file *filp, char *buf, size_t count,
		loff_t *offp)
{
    int len = 0;
    len += sprintf(buf + len, "LED status: %d\n", led_status);
    len += sprintf(buf + len, "Blink period: %d ms\n", blink_period_ms);
    return len;
}

static ssize_t led_proc_write(struct file *file, const char *buf,
                          size_t count, loff_t *offp)
{
    int period;
    if (sscanf(buf, "%d", &period) != 1) {
        return -EINVAL;
    }
    blink_period_ms = period;
    return count;
}

static const struct proc_ops led_proc_fops = {
	.proc_read = led_proc_read,
	.proc_write = led_proc_write,
};

static void blink_timer_func(struct timer_list *unused)
{
    gpio_set_value(LED, led_status);
    led_status = !led_status;
    blink_timer.expires = jiffies + msecs_to_jiffies(blink_period_ms);
    add_timer(&blink_timer);
}

static int __init led_init(void)
{
    int ret = 0;

    ret = gpio_request(LED, "led1");
    if (ret < 0) {
        printk(KERN_ERR "Unable to request GPIO: %d\n", ret);
        return ret;
    }

    ret = gpio_direction_output(LED, led_status);
    if (ret < 0) {
        printk(KERN_ERR "Unable to set GPIO direction: %d\n", ret);
        gpio_free(LED);
        return ret;
    }

    timer_setup(&blink_timer, blink_timer_func, 0);
    blink_timer.expires = jiffies + msecs_to_jiffies(blink_period_ms);
    add_timer(&blink_timer);

    if (!proc_create(PROC_FILENAME, 0, NULL, &led_proc_fops)) {
        printk(KERN_ERR "Unable to create /proc/%s\n", PROC_FILENAME);
        gpio_free(LED);
        return -ENOMEM;
    }

    printk(KERN_INFO "LED blink module loaded\n");
    return 0;
}

static void __exit led_exit(void)
{
    del_timer_sync(&blink_timer);
    gpio_set_value(LED, 0);
    gpio_free(LED);
    remove_proc_entry(PROC_FILENAME, NULL);
    printk(KERN_INFO "LED blink module unloaded\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Anoshyn");
MODULE_DESCRIPTION("LED blink module");

