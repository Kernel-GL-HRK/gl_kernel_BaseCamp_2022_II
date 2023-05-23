#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/err.h>
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <linux/ktime.h>
#include <linux/delay.h>
#include <asm-generic/div64.h>
#include "timer_and_irq.h"
#include "platform_driver.h"

MODULE_LICENSE("GPL");

static struct timer_list trigger_running;
static struct ultrasound_desc gpio_of_ultrasound;
static ktime_t start_echo, end_echo;
static int32_t distance, isToggled = 0, lastDistance;

irqreturn_t irq_handler_echo(int irq, void *dev_id)
{
    int64_t duration;
	int32_t gpio_status;

	gpio_status = gpio_get_value(gpio_of_ultrasound.echo_pin);

	if (isToggled == gpio_status)
		return IRQ_HANDLED;
	
    switch (gpio_status) {
    case ON:
        start_echo = ktime_get();
        break;
    case OFF:
        end_echo = ktime_get();
        duration = ktime_to_us(ktime_sub(end_echo, start_echo));
        
        if (duration > 0) {
            do_div(duration, 58);
            distance = (duration <= 400) ? duration : 401;
        }
		if (((distance - lastDistance) > NOISE_DISTANCE) || (((lastDistance - distance) > NOISE_DISTANCE)))
			lastDistance = distance;
		else
		 	distance = lastDistance;
        break;
    }
	isToggled = gpio_status;

    return IRQ_HANDLED;
}

void send_trig_signal(struct timer_list *timer)
{
	gpio_set_value(gpio_of_ultrasound.trig_pin, ON);
	ndelay(10000);
	gpio_set_value(gpio_of_ultrasound.trig_pin, OFF);

	mod_timer(&trigger_running, jiffies + msecs_to_jiffies(PERIOD_FOR_ULTRASOUND_MSEC));
}

int32_t run_ultrasound(void)
{
	int32_t echo_irq;
	int32_t error;
	
	gpio_of_ultrasound = get_ultra_description();
	if (!strcmp(gpio_of_ultrasound.status, "disconnected")) {
		pr_err("ultrasound-interrupts: ultrasound is disconnected\n");
		return -EINVAL;
	}

	{//
		error = echo_irq = gpio_to_irq(gpio_of_ultrasound.echo_pin);
		if (error < 0) {
			pr_err("ultrasound-interrupts-irq: irq pin is not aviable\n");
			return error;
		}

		error = request_irq(echo_irq, irq_handler_echo, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, NAME_OF_INTERRUPT_IRQ, NULL);
		if (error < 0) {
			pr_err("ultrasound-interrupts-irq: can not request irq\n");
			return error;
		}
	}
	{//Running timer for trigger of ultrasound
		timer_setup(&trigger_running, send_trig_signal, 0);
		mod_timer(&trigger_running, jiffies + msecs_to_jiffies(PERIOD_FOR_ULTRASOUND_MSEC));
	}
	pr_info("ultrasound-interrupts: ultrasound is running: %d %d\n", gpio_of_ultrasound.echo_pin, gpio_of_ultrasound.trig_pin);
	return 0;
}

void stop_ultrasound(void)
{
	del_timer(&trigger_running);
	free_irq(gpio_to_irq(gpio_of_ultrasound.echo_pin), NULL);
}

uint32_t get_distance(void)
{
	return distance;
}

