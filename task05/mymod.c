#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

#define DEVICE_NAME "mymod"

static int __init init_function(void)
{
	pr_info("%s: %s - %s:%d\n", DEVICE_NAME, "INIT", __func__, __LINE__);
	return 0;
}

static void __exit exit_function(void)
{
	pr_info("%s: %s - %s:%d\n", DEVICE_NAME, "EXIT", __func__, __LINE__);
}


module_init(init_function);
module_exit(exit_function);

MODULE_AUTHOR("Yurii Klysko");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simle kernel module.");
