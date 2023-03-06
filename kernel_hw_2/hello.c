// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kostiantyn Makhno");
MODULE_DESCRIPTION("Simple led driver");
MODULE_VERSION("1.0");

#define LED_PIN 23UL
#define PERIOD  1000UL

static struct timer_list tm;
static unsigned int led_state;

void tm_callback(struct timer_list *ptm)
{
	led_state = (~led_state) & 0x1;
	gpio_set_value(LED_PIN, led_state);
	mod_timer(&tm, jiffies + msecs_to_jiffies(PERIOD));
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

	if (gpio_direction_output(LED_PIN, led_state) != 0) {
		pr_info("can not set direction on %ld pin\n", LED_PIN);
		goto err_set_dir_gpio;
	}

	timer_setup(&tm, tm_callback, 0);
	mod_timer(&tm, jiffies + msecs_to_jiffies(PERIOD));

	return 0;

err_set_dir_gpio:
err_req_gpio:
	gpio_free(LED_PIN);
err_valid_gpio:
	return -EINVAL;
}

static void __exit hello_exit(void)
{
	del_timer(&tm);
	gpio_set_value(LED_PIN, 0);
	gpio_free(LED_PIN);
	pr_info("led pin %ld was freed\n", LED_PIN);
}

module_init(hello_init);
module_exit(hello_exit);
