#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

#define DEVICE_NAME "mymod"

static int num_1, num_2;
module_param(num_1, int, 0400);
module_param(num_2, int, 0400);

MODULE_PARM_DESC(num_1, "1st value");
MODULE_PARM_DESC(num_2, "2nd value");

static int __init init_function(void)
{
	pr_info("%s: %s - %s:%d\n", DEVICE_NAME, "INIT", __func__, __LINE__);
	pr_info("%s: %d + %d = %d\n", DEVICE_NAME, num_1, num_2, num_1 + num_2);
	pr_info("%s: %d - %d = %d\n", DEVICE_NAME, num_1, num_2, num_1 - num_2);
	pr_info("%s: %d * %d = %d\n", DEVICE_NAME, num_1, num_2, num_1 * num_2);
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
