#include <linux/kernel.h> 
#include <linux/module.h>

int init_module(void)
{
    printk("Hello, world\n");
    return 0;
}

void cleanup_module(void)
{
    printk("Goodbye\n");
}
