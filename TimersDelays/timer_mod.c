#include <linux/module.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

static struct timer_list my_timer;
u64 start_time;
/**
 * @brief Timer handler callback
 * 
 * @param data 
 */
void timer_callback(struct timer_list * data) {
	pr_info("Timer tick: %d\n", jiffies_to_msecs(jiffies - start_time));
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(986));
}

/**
 * @brief Func is called when module is loaded
 */
static int __init modInit(void) {
    printk("Timer module has started\n");

    /* Initialize timer */
	timer_setup(&my_timer, timer_callback, 0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(996));
    start_time = jiffies;
    pr_info("Timer has started\n");
    return 0;
}

/**
 * @brief Func is called when module removed
 */
static void __exit modExit(void) {
    printk("Timer module has removed\n");
    del_timer(&my_timer);
}

module_init(modInit);
module_exit(modExit);

/*Module information*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ME");
MODULE_DESCRIPTION("Timer example module");
MODULE_VERSION("0.1");
