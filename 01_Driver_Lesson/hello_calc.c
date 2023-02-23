#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/err.h>


static int num1;
static int num2;
static char *operate = "+";

module_param(num1, int, 0644);
MODULE_PARM_DESC(num1, " Integer. Example num1=5");
module_param(num2, int, 0644);
MODULE_PARM_DESC(num2, " Integer. Example num2=3");
module_param(operate, charp, 0644);
MODULE_PARM_DESC(operate, "Operation on numbers \"+\",\"-\",\"*\". Example operate=\"+\"");

static int __init hello_calc_init(void)
{
	int result = 0;

	pr_info("HELLO: Hello Kernel!\n");
	switch (operate[0]) {
	case '+':
		result = num1 + num2;
		pr_info("%d + %d = %d\n", num1, num2, result);
		break;
	case '-':
		result = num1 - num2;
		pr_info("%d - %d = %d\n", num1, num2, result);
		break;
	case '*':
		result = num1 * num2;
		pr_info("%d * %d = %d\n", num1, num2, result);
		break;
	default:
		pr_info("Invalid input %s: operation should be a `-`,`+`,`*`\n", operate);
		break;
	}

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
MODULE_DESCRIPTION("Welcome text and simple calculator driver for RaspberryPi Zero W\n\t\tExample: sudo insmod hello_calc.ko num1=3 operate=\"*\" num2=5\n\t\t\t sudo rmmod hello_calc.ko");
MODULE_VERSION("0.1.1");
