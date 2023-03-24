/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Leds GPIO's driver for RGB leds header
 *
 *
 * Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#ifndef __PROJECT1_H_INCLUDED
#define __PROJECT1_H_INCLUDED

#include <linux/gpio.h>
#include <linux/types.h>
#include <linux/ioctl.h>


/* Use shared buffer 8 byte */
#define BUFFER_MAX_SIZE		128
extern unsigned char fs_buffer[BUFFER_MAX_SIZE];

/*default GPIO values*/

/*
struct gpio {
	unsigned	gpio;
	unsigned long	flags;
	const char	*label;
};
*/

static struct gpio leds_gpios[] = {
		{ 17, GPIOF_OUT_INIT_LOW,  "Red LED" }, /* default to OFF */
		{ 27, GPIOF_OUT_INIT_LOW,  "Green LED"   }, /* default to OFF */
		{ 22, GPIOF_OUT_INIT_LOW,  "Blue LED"  }, /* default to OFF */
	};

/*definitions in dt*/
static const char *led_name_dt[] = { "gpio_red", "gpio_green", "gpio_blue"};
static const char *gpio_direction_dt[] = { "gpio_direction_red", "gpio_direction_green", "gpio_direction_blue"};
static const char *directions[] = {"out", "out", "out"};


/*****************************************************************************
* Global variable or extern global variabls/functions
*****************************************************************************/
extern unsigned char fs_buffer[BUFFER_MAX_SIZE];
extern size_t fs_buffer_size;
extern int leds_control(char *);
extern void setup_timers(void);
extern void leds_control_stop(void);
extern int leds_rgb_enable_procfs(void);
extern void leds_rgb_exit_procfs(void);

struct blink_data  {
	uint8_t led_num[ARRAY_SIZE(leds_gpios)];
	uint32_t blink_ms[ARRAY_SIZE(leds_gpios)];
	struct timer_list blinking_timer[ARRAY_SIZE(leds_gpios)];
};

extern struct blink_data blink;




#endif
