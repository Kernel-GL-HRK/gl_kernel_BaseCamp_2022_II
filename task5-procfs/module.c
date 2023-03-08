#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Serhii Shramko");
MODULE_DESCRIPTION("Simple LED blink driver with procfs");
MODULE_VERSION("0.1");

#define GPIO_LED_BLUE 23
#define GPIO_LED_RED 24
#define GPIO_LED_GREEN 25
static int current_led_on;
static bool is_led_on;

#define TIMEOUT 500 // milliseconds

static struct timer_list led_timer;

void timer_callback(struct timer_list *data)
{
	if (is_led_on) {
		gpio_set_value(current_led_on, 0);
		is_led_on = false;
		goto restart_timer;
	}

	switch (current_led_on) {
	case GPIO_LED_BLUE:
		current_led_on = GPIO_LED_RED;
		break;
	case GPIO_LED_RED:
		current_led_on = GPIO_LED_GREEN;
		break;
	case GPIO_LED_GREEN:
		current_led_on = GPIO_LED_BLUE;
		break;
	default:
		current_led_on = GPIO_LED_BLUE;
	}

	gpio_set_value(current_led_on, 1);
	is_led_on = true;

restart_timer:
	mod_timer(&led_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}

#define PROC_BUFFER_MAX_SIZE 512

static char procfs_buffer[PROC_BUFFER_MAX_SIZE] = { 0 };
static size_t procfs_buffer_size;

#define PROC_FILE_NAME "led_module"
static struct proc_dir_entry *proc_file;

static bool write_one_state(const char *fmt, int value)
{
	int need_size = snprintf(NULL, 0, fmt, value);

	if (procfs_buffer_size + need_size >= PROC_BUFFER_MAX_SIZE)
		return false;

	procfs_buffer_size +=
		sprintf(procfs_buffer + procfs_buffer_size, fmt, value);
	return true;
}

static void write_procfs_current_state(void)
{
	int value;

	procfs_buffer_size = 0;
	value = gpio_get_value(GPIO_LED_BLUE);
	if (!write_one_state("Blue led value:  %d\n", value))
		return;
	value = gpio_get_value(GPIO_LED_RED);
	if (!write_one_state("Red led value:   %d\n", value))
		return;
	value = gpio_get_value(GPIO_LED_GREEN);
	if (!write_one_state("Green led value: %d\n", value))
		return;
	value = TIMEOUT;
	if (!write_one_state("Timer blink period: %d milliseconds\n", value))
		return;
}

static ssize_t procfs_read_handler(struct file *File, char __user *buffer,
				   size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (*offset == 0)
		write_procfs_current_state();

	if (*offset >= procfs_buffer_size)
		return 0;

	to_copy = min(count, procfs_buffer_size - (size_t)(*offset));

	not_copied = copy_to_user(buffer, procfs_buffer + (*offset), to_copy);

	delta = to_copy - not_copied;
	*offset += delta;

	return delta;
}

static struct proc_ops fops = { .proc_read = procfs_read_handler };

static int __init led_init(void)
{
	pr_info("led_module: LED module started\n");

	if (gpio_is_valid(GPIO_LED_BLUE) == false) {
		pr_err("led_module: Error: GPIO_LED_BLUE %d is not valid",
		       GPIO_LED_BLUE);
		goto exit;
	}
	if (gpio_is_valid(GPIO_LED_RED) == false) {
		pr_err("led_module: Error: GPIO_LED_RED %d is not valid",
		       GPIO_LED_RED);
		goto exit;
	}
	if (gpio_is_valid(GPIO_LED_GREEN) == false) {
		pr_err("led_module: Error: GPIO_LED_GREEN %d is not valid",
		       GPIO_LED_GREEN);
		goto exit;
	}

	if (gpio_request(GPIO_LED_BLUE, "GPIO_LED_BLUE") < 0) {
		pr_err("led_module: Error: GPIO_LED_BLUE %d failed to request",
		       GPIO_LED_BLUE);
		goto exit;
	}
	if (gpio_request(GPIO_LED_RED, "GPIO_LED_RED") < 0) {
		pr_err("led_module: Error: GPIO_LED_RED %d failed to request",
		       GPIO_LED_RED);
		goto gpio_led_blue_exit;
	}
	if (gpio_request(GPIO_LED_GREEN, "GPIO_LED_GREEN") < 0) {
		pr_err("led_module: Error: GPIO_LED_GREEN %d failed to request",
		       GPIO_LED_GREEN);
		goto gpio_led_red_exit;
	}

	gpio_direction_output(GPIO_LED_BLUE, 0);
	gpio_direction_output(GPIO_LED_RED, 0);
	gpio_direction_output(GPIO_LED_GREEN, 0);

	gpio_set_value(GPIO_LED_BLUE, 1);
	current_led_on = GPIO_LED_BLUE;
	is_led_on = true;

	timer_setup(&led_timer, timer_callback, 0);
	mod_timer(&led_timer, jiffies + msecs_to_jiffies(TIMEOUT));

	proc_file = proc_create(PROC_FILE_NAME, 0644, NULL, &fops);
	if (!proc_file) {
		pr_err("led_module: Error: Could not initialize /proc/%s\n",
		       PROC_FILE_NAME);
		goto timer_exit;
	}
	pr_info("led_module: Driver setup done");

	return 0;

timer_exit:
	del_timer(&led_timer);
	gpio_free(GPIO_LED_GREEN);
gpio_led_red_exit:
	gpio_free(GPIO_LED_RED);
gpio_led_blue_exit:
	gpio_free(GPIO_LED_BLUE);
exit:
	return -1;
}

static void __exit led_exit(void)
{
	gpio_set_value(GPIO_LED_BLUE, 0);
	gpio_free(GPIO_LED_BLUE);

	gpio_set_value(GPIO_LED_RED, 0);
	gpio_free(GPIO_LED_RED);

	gpio_set_value(GPIO_LED_GREEN, 0);
	gpio_free(GPIO_LED_GREEN);

	del_timer(&led_timer);
	proc_remove(proc_file);

	pr_info("led_module: LED module exit\n");
}

module_init(led_init);
module_exit(led_exit);
