#include <linux/kernel.h> 
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roman Kulchytskyi");
MODULE_DESCRIPTION("Some description");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
    printk("Hello, world\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk("Goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);
