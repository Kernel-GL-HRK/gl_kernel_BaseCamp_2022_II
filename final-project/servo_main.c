#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/err.h>
#include "sys_dev.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muravka Roman");
MODULE_DESCRIPTION("Driver for servo with devFS, procFS, Device Tree");
MODULE_VERSION("0.1");

static int servo_init(void)
{
   pr_info("servo: Creating /dev file system\n");
   return create_devFS(); 
}

static void servo_exit(void) 
{
   pr_info("servo: Removing /dev file system\n");
   remove_devFS();
}

module_init(servo_init);
module_exit(servo_exit);
