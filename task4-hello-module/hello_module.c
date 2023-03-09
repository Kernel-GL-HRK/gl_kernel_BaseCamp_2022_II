// SPDX-License-Identifier: GPL-2.0

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Viacheslav Us");
MODULE_DESCRIPTION("First hello kernel module");
MODULE_VERSION("0.2");

static int num1;
static int num2;

module_param(num1, int, 0444);
MODULE_PARM_DESC(num1, "First value");

module_param(num2, int, 0444);
MODULE_PARM_DESC(num2, "Second value");

static int __init hello_init(void)
{
	pr_info("Hello World!\n");

	pr_info("%d + %d = %d\n", num1, num2, num1 + num2);
	pr_info("%d - %d = %d\n", num1, num2, num1 - num2);
	pr_info("%d * %d = %d\n", num1, num2, num1 * num2);
	
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Goodbye World!\n");
}

module_init(hello_init);
module_exit(hello_exit);


