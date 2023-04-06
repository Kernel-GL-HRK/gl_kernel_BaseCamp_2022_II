#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/sched.h>

static int __init my_init(void)
{
    unsigned long start_time = jiffies;
    unsigned long wait_time = msecs_to_jiffies(50); // Wait for 1 second

    while (!time_after_eq(jiffies, start_time + wait_time)) {
        printk("waiting time after...\n");
        schedule();
    }
    start_time = jiffies;
    while (time_before(jiffies, start_time + wait_time)) {
        cpu_relax();
        printk("waiting time before...\n");
    }
    
        
    printk("Waited for %ld seconds\n", 2 * wait_time / HZ);

    return 0;
}

static void __exit my_exit(void)
{
    printk("Module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Oleksandr Posukhov <alex.posukhov@gmail.com>");
MODULE_DESCRIPTION("Time_after, time_before example module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");