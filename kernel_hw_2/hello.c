#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple led driver");

#define LED_PIN 23UL 

static struct timer_list tm;

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
	
	if (gpio_direction_output(LED_PIN, 0) != 0) {
		pr_info("can not set direction on %ld pin\n", LED_PIN);
		goto err_set_dir_gpio;
	}

//	timer_setup(&tm, tm_callback, 0);
//	mod_timer(&tm, jiffies + msecs_to_jiffies(TIMEOUT));

	return 0;

err_set_dir_gpio:
err_req_gpio:
	gpio_free(LED_PIN);
err_valid_gpio:
	return -EINVAL;
}

static void __exit hello_exit(void)
{
//	del_timer(&tm);
	gpio_set_value(LED_PIN, 0);
	gpio_free(LED_PIN);
	pr_info("led pin %ld was freed\n", LED_PIN);
}

module_init(hello_init);
module_exit(hello_exit);
