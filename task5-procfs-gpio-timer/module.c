// SPDX-License-Identifier: GPL
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/err.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kushnir Vladyslav");
MODULE_DESCRIPTION("Procfs, GPIO, timer...");
MODULE_VERSION("0.1");

#define GPIO_21 (21)

#define TIMEOUT (1000 / 20)
static struct timer_list etx_timer;
static unsigned int count;
static bool status;

static void timer_callback(struct timer_list *data)
{
	count++;
	status = !status;

	gpio_set_value(GPIO_21, status);

	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}

static int __init init(void)
{
	pr_info("The 'Procfs, GPIO, timer...' module starts initalization.");

	if (!gpio_is_valid(GPIO_21)) {
		pr_err("ERROR: GPIO %d is not valid\n", GPIO_21);
		goto exit;
	}

	if (gpio_request(GPIO_21, "GPIO_21") < 0) {
		pr_err("ERROR: GPIO %d request\n", GPIO_21);
		goto exit;
	}

	gpio_direction_output(GPIO_21, 0);


	timer_setup(&etx_timer, timer_callback, 0);

	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));

	pr_info("The initalization of module 'Procfs, GPIO, timer...' is successeful.");
	return 0;

gpio_exit:
	gpio_free(GPIO_21);

exit:
	return -1;
}

static void __exit exit_module(void)
{
	del_timer(&etx_timer);

	gpio_set_value(GPIO_21, 0);

	gpio_free(GPIO_21);

	pr_info("The work of the 'first_module' module is completed.\n");
}

module_init(init);
module_exit(exit_module);
