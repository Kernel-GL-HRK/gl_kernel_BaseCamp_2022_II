#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>


static int condition = 0;
static struct wait_queue_head my_wait_queue;

static int my_thread(void *arg)
{
    // Wait for 3 seconds
    set_current_state(TASK_INTERRUPTIBLE);
    schedule_timeout(3 * HZ);
    printk("Condition arrived\n");
    // Set condition to true
    condition = 1;

    // Wake up any threads waiting on the wait queue
    wake_up_interruptible(&my_wait_queue);

    return 0;
}

static int __init my_init(void)
{
    unsigned long timeout = 5 * HZ; // Timeout after 5 seconds
    struct task_struct *task;
    
    printk("Module loaded\n");

    init_waitqueue_head(&my_wait_queue);

    // Start a new thread to modify condition
    task = kthread_create(my_thread, NULL, "my_thread");
    if (IS_ERR(task)) {
        printk(KERN_ERR "Failed to create thread\n");
        return PTR_ERR(task);
    }

    //wake_up_process(task);


    // Wait for the condition to become true or timeout
    while (!condition) {
        int ret = wait_event_interruptible_timeout(my_wait_queue, condition, timeout);

        if (ret == 0) {
            // Timeout occurred
            printk(KERN_INFO "Timeout occurred\n");
            break;
        } else if (ret < 0) {
            // Interrupted by a signal
            printk(KERN_INFO "Interrupted by a signal\n");
            return -EINTR;
        }
    }

    if (condition) {
        printk(KERN_INFO "Condition is true\n");
    }

    return 0;
}

static void __exit my_exit(void)
{
    printk(KERN_INFO "Module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

/*Module information*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ME");
MODULE_DESCRIPTION("Wait queue example module");
MODULE_VERSION("0.1");