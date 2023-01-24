// SPDX-License-Identifier: GPL-2.0+

/*
 * encoder_driver.c - simple encoder driver for linux kernel
 */

#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

// Pins with encoder
#define GPIO_PIN_A 7
#define GPIO_PIN_B 8

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Yurii.Bezkor");
MODULE_DESCRIPTION("Encoder driver");
MODULE_VERSION("0.1");

int encoder_value; // value encoder;

unsigned int gpio_irq;

// Register params
module_param(encoder_value, int, 0664);

// IRQ handler
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
	static unsigned long flags;

	local_irq_save(flags);

	if (gpio_get_value(GPIO_PIN_B))
		encoder_value++;
	else
		encoder_value--;

	pr_info("encoder_value = %d\n", encoder_value);

	local_irq_restore(flags);

	return IRQ_HANDLED;
}

/*
 * Module init function
 */
static int __init encoder_init(void)
{
	encoder_value = 0;
	// Check gpio inputs
	if (gpio_is_valid(GPIO_PIN_A) && gpio_is_valid(GPIO_PIN_B)) {
		// Config pins as input
		gpio_direction_input(GPIO_PIN_A);
		pr_info("Config A = %d\n", GPIO_PIN_A);

		gpio_direction_input(GPIO_PIN_B);
		pr_info("Config B = %d\n", GPIO_PIN_B);

		// Config pin A as IRQ pin
		gpio_irq = gpio_to_irq(GPIO_PIN_A);
		pr_info("Config IRQ = %d\n", gpio_irq);

		// Try register IRQ
		if (request_irq(gpio_irq, (void *)gpio_irq_handler,
				IRQF_TRIGGER_FALLING, "encoder_device", NULL)) {
			pr_info("Error IRQ\n");
		}
	}

	pr_info("Encoder Kernel Module Inserted Successfully...\n");
	return 0;
}

/*
 * Module Exit function
 */
static void __exit encoder_exit(void)
{
	free_irq(gpio_irq, NULL);
	gpio_free(GPIO_PIN_A);
	gpio_free(GPIO_PIN_B);
	pr_info("Encoder Kernel Module Removed Successfully...\n");
}

module_init(encoder_init);
module_exit(encoder_exit);
