// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module that uses procfs ang gpio");
MODULE_AUTHOR("Tereshchenko Dmytro <buxdmo@gmail.com>");
MODULE_VERSION("1.0");

#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

/*===============================================================================================*/
static int driver_init(void)
{
	pr_info("module init");
	return 0;
}
/*===============================================================================================*/
static void driver_exit(void)
{
	pr_info("module exit");
}
/*===============================================================================================*/

module_init(driver_init);
module_exit(driver_exit);
