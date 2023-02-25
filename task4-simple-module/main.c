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

/*===============================================================================================*/
static int driver_init(void)
{
	pr_notice("module_init\n");
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
