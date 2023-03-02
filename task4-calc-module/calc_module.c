#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Serhii Shramko");
MODULE_DESCRIPTION("Simple module calculator");
MODULE_VERSION("0.1");

static int n1;
module_param(n1, int, 0);

static int n2;
module_param(n2, int, 0);

static int __init calc_init(void)
{
	pr_info("Module calculator start\n");
	pr_info("    Sum: %d + %d = %d\n", n1, n2, n1 + n2);
	pr_info("    Substraction: %d - %d = %d\n", n1, n2, n1 - n2);
	pr_info("    Multiplication: %d * %d = %d\n", n1, n2, n1 * n2);
	return 0;
}

static void __exit calc_exit(void)
{
	pr_info("Module calculator exit\n");
}

module_init(calc_init);
module_exit(calc_exit);
