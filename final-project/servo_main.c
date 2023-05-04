#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/err.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muravka Roman");
MODULE_DESCRIPTION("Driver for servo with devFS, procFS, Device Tree");
MODULE_VERSION("0.0");

static int servo_init(void)
{
   return 0; 
}

static void servo_exit(void) 
{
}

module_init(servo_init);
module_exit(servo_exit);
