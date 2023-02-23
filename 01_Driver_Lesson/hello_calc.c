#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/err.h>

static int __init hello_calc_init(void)
{

	pr_info("HELLO: Hello Kernel!\n");

return 0;
}
static void __exit hello_calc_exit(void)
{
	pr_info("HELLO: Goodbye Kernel!\n");
}
module_init(hello_calc_init);
module_exit(hello_calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mykhailo Vyshnevskyi");
MODULE_DESCRIPTION("Welcome text");
MODULE_VERSION("0.1.0");
