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
MODULE_VERSION("0.2");

int encoder_value; // value encoder

unsigned int gpio_irq; // IRQ number

// Register params
module_param(encoder_value, int, 0664);

/*
 * IRQ handler
 */
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
	// Error code variable
	int error_code;

	// Check is GPIO valid
	if (false == gpio_is_valid(GPIO_PIN_A)) {
		pr_err("GPIO_PIN_A = %d not valid\n", GPIO_PIN_A);
		return -EINVAL;
	}
	pr_info("GPIO_PIN_A = %d is valid\n", GPIO_PIN_A);

	if (false == gpio_is_valid(GPIO_PIN_B)) {
		pr_err("GPIO_PIN_B = %d not valid\n", GPIO_PIN_B);
		return -EINVAL;
	}
	pr_info("GPIO_PIN_B = %d is valid\n", GPIO_PIN_B);

	// Try GPIO request
	error_code = gpio_request(GPIO_PIN_A, "Pin A");
	if (error_code) {
		pr_err("GPIO_PIN_A request error\n");
		return error_code;
	}
	pr_info("GPIO_PIN_A = %d request is OK\n", GPIO_PIN_A);

	error_code = gpio_request(GPIO_PIN_B, "Pin B");
	if (error_code) {
		pr_err("GPIO_PIN_B request error\n");
		return error_code;
	}
	pr_info("GPIO_PIN_B = %d request is OK\n", GPIO_PIN_B);

	// Config pins as input
	gpio_direction_input(GPIO_PIN_A);
	pr_info("Config A = %d\n", GPIO_PIN_A);
	gpio_direction_input(GPIO_PIN_B);
	pr_info("Config B = %d\n", GPIO_PIN_B);

	// Config pin A as IRQ pin
	gpio_irq = gpio_to_irq(GPIO_PIN_A);
	pr_info("Config IRQ = %d\n", gpio_irq);

	// Try register IRQ
	error_code = request_irq(gpio_irq, (void *)gpio_irq_handler,
				 IRQF_TRIGGER_FALLING, "encoder_device", NULL);
	if (error_code) {
		pr_info("Error IRQ request\n");
		return error_code;
	}
	pr_info("IRQ request is OK\n");

	// Init default encoder value
	encoder_value = 0;
	pr_info("encoder_value = %d\n", encoder_value);

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

/*
 * NOTE:
 * Init procedure of GPIO
 * gpio_is_valid -> gpio_request -> gpio_direction_input
 * can be replaced by a cleaner and more correct call
 * gpio_request_one or gpio_request_array
 */
