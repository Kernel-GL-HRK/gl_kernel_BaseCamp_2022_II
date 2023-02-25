// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple kernel module");
MODULE_AUTHOR("Tereshchenko Dmytro <buxdmo@gmail.com>");
MODULE_VERSION("1.0");

#ifdef pr_fmt
	#undef pr_fmt
#endif

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

static int param1;
static int param2;

module_param(param1, int, 0660);
module_param(param2, int, 0660);

MODULE_PARM_DESC(param1, "First parameter");
MODULE_PARM_DESC(param2, "Second parameter");

/*===============================================================================================*/
static int driver_init(void)
{
	pr_notice("module_init\n");
	pr_info("The sum of %d and %d is %d\n", param1, param2, param1 + param2);
	pr_info("The diff of %d and %d is %d\n", param1, param2, param1 - param2);
	pr_info("The product of %d and %d is %d\n", param1, param2, param1 * param2);
	return 0;
}
/*===============================================================================================*/
static void driver_exit(void)
{
	pr_notice("module_exit\n");
}

/*===============================================================================================*/
module_init(driver_init);
module_exit(driver_exit);
