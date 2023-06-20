// SPDX-License-Identifier: GPL-2.0-only
/*
 * Module library: sc_button
 * Description: GPIO button controls for the module
 * Module: smart-clock
 *
 * Copyright (C) 2023 Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#include <linux/gpio.h>     //GPIO
#include <linux/export.h>  //export project class name
#include <linux/types.h>
#include <linux/timer.h>  //timer
#include <linux/interrupt.h> //irq trigger
#include "include/project1.h"


#define TRIGGER_IRQ "button_trigger"
#define DEVICE_NAME "gpio_button"

#define BUTTON_DEBOUNCE_INTERVAL 150
#define BUTTON_LONGPRESS_INTERVAL 500
#define BUTTON_2ND_LONGPRESS_INTERVAL 800

/**
 * init GPIO button with values to avoid troubles. But later replace from DT
 *
 * struct gpio {
 *	unsigned	gpio;
 *	unsigned long	flags;
 *	const char	*label;
 * };
 */

static struct gpio button_gpio = { 17, GPIOF_DIR_IN,  "Switch button"};

/* IRQ number will be assigned by request_irq for GPIO button press events */
static uint8_t button_irq;

/* flags: to store IRQ flags, counter: and longpress_counter: for long press false detection */
static unsigned long flags = 0, counter = 0, longpress_counter;

/* Timers will be used for press debounce and long press detections */
static struct timer_list bDebounce_timer, bLongpress_timer;

/* Will be true, while debounce timer running */
static uint8_t is_debounce_timer;


/**
 * switch_view() - the main view switch by @my_button.view_mode value
 *
 * switches view to clock mode and runs timer
 */
static void switch_view(void)
{
	my_button.view_mode++;
	my_button.state = 0;

	/* Erase vmem before switching the view */
	st7735fb_blank_vmem();
	
	/* Switch view */
	switch (my_button.view_mode) {
	case CLOCK:
		show_clock_view();
		break;
	case TIMER:
		show_timer_view();
		break;
	case ALARM:
		show_alarm_view();
		break;
	case TEMP_AND_PRESS:
		show_temp_and_press_view();
		break;
	case PEDOMETER:
		show_pedometer_view();
		break;
	case GAME:
		show_game_view();
		break;
	case OPTIONS:
		show_options_view();
		break;
	case DISP_MODES_END:
		my_button.view_mode = 0;
		switch_view();
		break;
	}

}

/**
 * button_workqueue_fn() - workqueue started after button interrrupt triggered
 *
 * starts debounce and long press timers
 */
void button_workqueue_fn(struct work_struct *work)
{
	if (!my_button.is_longpress)
		mod_timer(&bDebounce_timer,
			jiffies + msecs_to_jiffies(BUTTON_DEBOUNCE_INTERVAL));
	mod_timer(&bLongpress_timer,
		jiffies + msecs_to_jiffies(BUTTON_LONGPRESS_INTERVAL));

	longpress_counter = counter;
		}


DECLARE_WORK(button_workqueue, button_workqueue_fn);


/**
 * button_longpress_timer() - longpress detection timer
 *
 * there are two states of long press @my_button.state =2 and @my_button.state=3
 * following will detect it.
 */
static void button_longpress_timer(struct timer_list *t)
{
	if ((longpress_counter == counter) && gpio_get_value(button_gpio.gpio)) {
		if (my_button.is_longpress) {
			my_button.state = 3;
			if (!my_button.edit_mode)
				switch_view();
		} else
			my_button.state = 2;

	my_button.is_longpress = 1;
	mod_timer(&bLongpress_timer,
		jiffies + msecs_to_jiffies(BUTTON_2ND_LONGPRESS_INTERVAL));
	} else
		my_button.is_longpress = 0;

}

/**
 * button_debounce_timer() - shot press/debounce timer
 *
 */
static void button_debounce_timer(struct timer_list *t)
{

	if (!gpio_get_value(button_gpio.gpio) && (!my_button.is_longpress))
		my_button.state = 1;
	else
		my_button.state = 0;
	is_debounce_timer = 0;
}

/**
 * irqreturn_t button_press() - button press interrupt trigger.
 *
 * schedules @button_workqueue_fn() workqueue
 */
static irqreturn_t button_press(int irq, void *data)
{
	local_irq_save(flags);

	counter++;
	if (!is_debounce_timer && !my_button.is_longpress)  {
		schedule_work(&button_workqueue);
		is_debounce_timer = 1;
				}
	local_irq_restore(flags);

	return IRQ_HANDLED;
}


/**
 * gpio_button_init() - init module library from main.
 *
 */
int  gpio_button_init(void)
{
	struct device_node *np;
	/*read GPIo value from dt */
	np = of_find_compatible_node(NULL, NULL, "smart-clock,button");
	if (np) {
		if (!of_property_read_u32(np, "button_gpio", &button_gpio.gpio))
			pr_err("%s: found gpio #%i value in DT\n", DEVICE_NAME, button_gpio.gpio);
		}

	if (!gpio_is_valid(button_gpio.gpio)) {
		pr_err("%s: gpio %d is not valid\n", DEVICE_NAME, button_gpio.gpio);
		return -1;
		}
	/* GPIO's request*/
	if (gpio_request_one(button_gpio.gpio, button_gpio.flags, button_gpio.label)) {
		pr_err("%s: Cannot request GPIO\n", DEVICE_NAME);
		return -1;
	}

		/* hiding gpio from sysfs */
		gpio_export(button_gpio.gpio, false);

		/* requesting IRQ for gpio handler */
		button_irq = gpio_to_irq(button_gpio.gpio);
			if (button_irq < 0) {
				pr_err("%s: unable to get irq number for GPIO %d, error %d\n", DEVICE_NAME,
					button_gpio.gpio, button_irq);
					gpio_free(button_gpio.gpio);
					return -1;
			}


		if (request_irq(button_irq, button_press, IRQF_TRIGGER_RISING, TRIGGER_IRQ, DEVICE_NAME)) {
			pr_err("%s: register IRQ %d, error\n", DEVICE_NAME,
					button_gpio.gpio);
					free_irq(button_irq, DEVICE_NAME);
					gpio_free(button_gpio.gpio);
					return -1;
		}

		/* setting up timers for debounce and long press #1 detection */
		timer_setup(&bDebounce_timer, button_debounce_timer, 0);
		timer_setup(&bLongpress_timer, button_longpress_timer, 0);
		is_debounce_timer = 0;
		my_button.state = 0;
		my_button.edit_mode = 0;

return 0;

}

/**
 *  gpio_button_unload() - exit module library from main.
 *
 */
void  gpio_button_unload(void)
{
	pr_err("%s: device exit\n", DEVICE_NAME);
	del_timer_sync(&bDebounce_timer);
	del_timer_sync(&bLongpress_timer);
	free_irq(button_irq, DEVICE_NAME);
	gpio_free(button_gpio.gpio);
	}


