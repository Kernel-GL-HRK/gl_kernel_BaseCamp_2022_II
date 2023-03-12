// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
#include "pr_fmt.h"
#include <linux/gpio.h>
#include <linux/timer.h>
#include "gpio.h"

static unsigned int GPIO_PIN;

static int gpio_status;
static int blink_status;

static struct timer_list blink_timer;

/*===============================================================================================*/
static void gpio_set_status(int status)
{
	if (gpio_status == status)
		return;

	gpio_status = status;
	gpio_set_value(GPIO_PIN, status);
}
/*===============================================================================================*/
unsigned int gpio_get_status(void)
{
	return gpio_status;
}
/*===============================================================================================*/
static void blink_timer_callback(struct timer_list *unused)
{
	gpio_set_status(!gpio_status);
	mod_timer(&blink_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}
/*===============================================================================================*/
void gpio_blink_on(void)
{
	if (blink_status)
		return;

	timer_setup(&blink_timer, blink_timer_callback, 0);
	mod_timer(&blink_timer, jiffies + msecs_to_jiffies(TIMEOUT));
	blink_status = 1;
}
/*===============================================================================================*/
void gpio_blink_off(void)
{
	if (!blink_status)
		return;

	del_timer_sync(&blink_timer);
	gpio_set_status(0);
	blink_status = 0;
}
/*===============================================================================================*/
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
/*===============================================================================================*/
void gpio_exit(void)
{
	gpio_blink_off();
	gpio_free(GPIO_PIN);
}
/*===============================================================================================*/
