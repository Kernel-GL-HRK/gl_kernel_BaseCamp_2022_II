#include <linux/gpio.h>
#include <linux/timer.h>
#include "gpio.h"

static unsigned int GPIO_PIN;

int gpio_init(unsigned int gpio)
{
	int err;

	if (!gpio_is_valid(gpio)) {
		pr_err("Gpio pin is not valid\n");
		return -EINVAL;
	}

	err = gpio_request_one(gpio, GPIOF_OUT_INIT_LOW, "pin");

	if (err) {
		pr_err("Could not request gpio: %d\n", err);
		return err;
	}

	GPIO_PIN = gpio;

	return 0;
}

void gpio_exit(void)
{
	gpio_blink_off();
	gpio_free(GPIO_PIN);
}
