// SPDX-License-Identifier: GPL-2.0-only
/*
 * Leds GPIO's driver for RGB leds.
 *
 * Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h> //timer for timer
#include <linux/jiffies.h>  //jiffies.h for timer
#include <linux/string.h>
#include "project1.h"

#define MODULE_NAME module_name(THIS_MODULE)

static uint8_t count;
static uint8_t define_led[ARRAY_SIZE(leds_gpios)] = {0};
struct blink_data blink;

/* blinking timer red led*/
static void red_blink(struct timer_list *timer)
{
	if (blink.led_num[0]) {
		gpio_set_value(leds_gpios[0].gpio, !gpio_get_value(leds_gpios[0].gpio));
		mod_timer(&blink.blinking_timer[0], jiffies + msecs_to_jiffies(blink.blink_ms[0]));
	} else {
		gpio_set_value(leds_gpios[0].gpio, 0);
		}

}
/* blinking timer green led*/
static void green_blink(struct timer_list *timer)
{
	if (blink.led_num[1]) {
		gpio_set_value(leds_gpios[1].gpio, !gpio_get_value(leds_gpios[1].gpio));
		mod_timer(&blink.blinking_timer[1], jiffies + msecs_to_jiffies(blink.blink_ms[1]));
	} else {
		gpio_set_value(leds_gpios[1].gpio, 0);
		}
}
/* blinking timer blue led*/
static void blue_blink(struct timer_list *timer)
{
	if (blink.led_num[2]) {
		gpio_set_value(leds_gpios[2].gpio, !gpio_get_value(leds_gpios[2].gpio));
		mod_timer(&blink.blinking_timer[2], jiffies + msecs_to_jiffies(blink.blink_ms[2]));
	} else {
		gpio_set_value(leds_gpios[2].gpio, 0);
		}
}

/* Turning on/off leds from devsysf buffer cmd */
int leds_control(char *buffer)
{
	uint8_t i = 0, led_triggered;
	char *buf_res, *delay;
	uint8_t buf_diff;
	int blink_delay;

	while (buffer[i] != '\0') {
		led_triggered = 0;
		switch (buffer[i]) {
		case 'r':
				led_triggered = 1;
				break;
		case 'g':
				led_triggered = 2;
				break;
		case 'b':
				led_triggered = 3;
				break;
		}
		if (led_triggered) {
			led_triggered -= 1;
			delay = &buffer[i+2];

			if (kstrtoint(delay, 10, &blink_delay) != 0) {
				pr_err("%s: Parsing of blinking delay from buffer failed\n", MODULE_NAME);
			return -EINVAL;
			}
			blink.led_num[led_triggered] = 1; //turning on current led and setting blinking delay or otherwise
			if (blink_delay)
				blink.blink_ms[led_triggered] = blink_delay;
			else {
				blink.led_num[led_triggered] = 0;
				blink.blink_ms[led_triggered] = 0;
				}
			mod_timer(&blink.blinking_timer[led_triggered], jiffies + msecs_to_jiffies(0));
		}

		i++;
	}
return 0;
}

/* Setup blinking timer */
void setup_timers(void)
{
	timer_setup(&blink.blinking_timer[0], red_blink, 0);
	timer_setup(&blink.blinking_timer[1], green_blink, 0);
	timer_setup(&blink.blinking_timer[2], blue_blink, 0);
}


/* Turning all leds off and removing timer*/
void leds_control_stop(void)
{
	for (count = 0; count < ARRAY_SIZE(leds_gpios); count++) {
		gpio_set_value(leds_gpios[count].gpio, 0);
		del_timer_sync(&blink.blinking_timer[count]);
	}
}


