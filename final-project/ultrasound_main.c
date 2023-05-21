#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/err.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muravka Roman");
MODULE_DESCRIPTION("Driver for ultrasound with devFS, procFS, Device Tree");
MODULE_VERSION("0.1");

static int ultrasound_init(void)
{
	return 0;
}

static void ultrasound_exit(void) 
{
}

module_init(ultrasound_init);
module_exit(ultrasound_exit);
