// SPDX-License-Identifier: GPL
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/err.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kushnir Vladyslav");
MODULE_DESCRIPTION("Procfs, GPIO, timer...");
MODULE_VERSION("0.1");

#define GPIO_21 (21)

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

	pr_info("The initalization of module 'Procfs, GPIO, timer...' is successeful.");
	return 0;

gpio_exit:
	gpio_free(GPIO_21);

exit:
	return -1;
}

static void __exit exit_module(void)
{
	gpio_set_value(GPIO_21, 0);

	gpio_free(GPIO_21);

	pr_info("The work of the 'first_module' module is completed.\n");
}

module_init(init);
module_exit(exit_module);
