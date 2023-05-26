// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
/*
 * Kernel module
 * GPIO Led-driver
 * GPIO 4 is visible in /sys/class/gpio/
 * You can change the gpio value (Led ON/OFF) by using below commands.
 * echo 1 > /sys/class/gpio/gpio4/value (turn ON the LED)
 * echo 0 > /sys/class/gpio/gpio4/value (turn OFF the LED)
 * cat /sys/class/gpio/gpio4/value (read the value LED)
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vadym Minziuk");
MODULE_DESCRIPTION("GPIO Led-driver");
MODULE_VERSION("0.1");

#define GPIO_4 (4) //LED is connected to this GPIO

static int gpio_pin_num;

static int __init gpio_led_driver_init(void)
{
	struct device_node *nd;

	nd = of_find_compatible_node(NULL, NULL, "my_module");

	if (!nd) {
		pr_err("ERROR: Device Tree problem. Node absent\n");
		goto exit;
	} else {
		gpio_pin_num = be32_to_cpup(of_get_property(nd, "gpio_pin_led", NULL));
		pr_info("gpio_pin_num = %d\n", gpio_pin_num);
	}

	if (gpio_is_valid(gpio_pin_num) == false) {
		pr_err("ERROR: GPIO %d is not valid\n", gpio_pin_num);
		goto exit;
	}

	if (gpio_request(gpio_pin_num, "GPIO_LED") < 0) {
		pr_err("ERROR: GPIO %d request\n", gpio_pin_num);
		goto exit;
	}

	gpio_direction_output(gpio_pin_num, 0);

	/* Using this call the GPIO gpio_pin_num will be visible in /sys/class/gpio/
	 * Now you can change the gpio value by using below commands also.
	 * echo 1 > /sys/class/gpio/gpio4/value (turn ON the LED)
	 * echo 0 > /sys/class/gpio/gpio4/value (turn OFF the LED)
	 * cat /sys/class/gpio/gpio4/value (read the value LED)
	 *
	 * The second argument prevents the direction from being changed
	 */

	gpio_export(gpio_pin_num, false);

	pr_info("Device Driver Insert... Done!!!\n");
	gpio_set_value(gpio_pin_num, 1);
	return 0;

//gpio_exit:
//	gpio_free(gpio_pin_num);

exit:
	return -1;
}

static void __exit gpio_led_driver_exit(void)
{
	gpio_set_value(gpio_pin_num, 0);
	gpio_unexport(gpio_pin_num);
	gpio_free(gpio_pin_num);

	pr_info("Device Driver Remove... Done!!!\n");
}

module_init(gpio_led_driver_init);
module_exit(gpio_led_driver_exit);
