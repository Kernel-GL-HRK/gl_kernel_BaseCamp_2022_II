#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple kernel module");
MODULE_AUTHOR("Tereshchenko Dmytro <buxdmo@gmail.com>");
MODULE_VERSION("1.0");

/*===============================================================================================*/
static int driver_init(void)
{
	pr_debug("module_init\n");
	return 0;
}
/*===============================================================================================*/
static void driver_exit(void)
{
	pr_debug("module_exit\n");
}

/*===============================================================================================*/
module_init(driver_init);
module_exit(driver_exit);
