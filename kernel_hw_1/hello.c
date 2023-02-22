#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple module");
MODULE_AUTHOR("Kostiantyn Makhno");
MODULE_VERSION("0.1");

static int op1;
static int op2;

module_param(op1, int, 0444);
MODULE_PARM_DESC(op1, "The first integer operand");
module_param(op2, int, 0444);
MODULE_PARM_DESC(op2, "The second integer operand");

static int __init hello_init(void)
{
	pr_info("hello module was initialized\n");
	pr_info("sum of operands: %d\n", op1 + op2);
	pr_info("diff of operands: %d\n", op1 - op2);
	pr_info("mult of operands: %d\n", op1 * op2);

	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("hello module was uninitialized\n");
}

module_init(hello_init);
module_exit(hello_exit);
