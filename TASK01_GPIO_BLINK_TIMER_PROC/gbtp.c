// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Yurii Bezkor");
MODULE_DESCRIPTION("ModuleParam feature");
MODULE_VERSION("0.2");

static int led_pin = -1;
module_param(led_pin, int, 0644);
MODULE_PARM_DESC(led_pin, "Led pin number for Blink");

static unsigned int timer_count = 1000;
module_param(timer_count, int, 0644);
MODULE_PARM_DESC(timer_count, "Timer period");

static struct timer_list timer_gbtp;

void timer_callback(struct timer_list *data)
{
	pr_info("GBTP: Timer call\n");
	mod_timer(&timer_gbtp, jiffies + msecs_to_jiffies(timer_count));
}

static int __init gbtp_init(void)
{
	pr_info("GBTP: LedPin = %d\n", led_pin);
	pr_info("GBTP: TimerC = %d\n", timer_count);

	timer_setup(&timer_gbtp, timer_callback, 0);
	mod_timer(&timer_gbtp, jiffies + msecs_to_jiffies(timer_count));

	pr_info("GBTP: Load success!\n");
	return 0;
}
static void __exit gbtp_exit(void)
{
	del_timer(&timer_gbtp);
	pr_info("GBTP: Unload done!\n");
}

module_init(gbtp_init);
module_exit(gbtp_exit);
