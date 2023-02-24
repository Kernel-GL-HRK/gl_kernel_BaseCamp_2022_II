#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");

static int num1 = 1;
static int num2 = 1;

module_param(num1, int, 0644);
MODULE_PARM_DESC(num1, "First integer");
module_param(num2, int, 0644);
MODULE_PARM_DESC(num2, "Second integer");

static int __init calcul_init(void)
{
	pr_info("Hi!\n");
	pr_info("Summation: %d\n", num1 + num2);
	pr_info("Difference: %d\n", num1 - num2);
	pr_info("Product: %d\n", num1 * num2);
	pr_info("Got: %d, %d\n", num1, num2);
	return 0;
}

static void __exit calcul_exit(void)
{
	pr_info("Goodbye!\n");
}

module_init(calcul_init);
module_exit(calcul_exit);
