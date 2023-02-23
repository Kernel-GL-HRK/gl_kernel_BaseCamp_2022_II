// SPDX-License-Identifier: GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergiy Us");
MODULE_DESCRIPTION("The simple calculator");
MODULE_VERSION("0.1");

static long par1;
static long par2;

module_param(par1, long, 0444);
MODULE_PARM_DESC(par1, "The first variable");

module_param(par2, long, 0444);
MODULE_PARM_DESC(par2, "The second variable");

static int __init hello_init(void)
{
	pr_info("HELLO: Hello linux kernel!\n");
	pr_info("CALC: par1 = %ld, par2 = %ld\n", par1, par2);
	pr_info("CALC: sum = %ld\n", par1 + par2);
	pr_info("CALC: sub = %ld\n", par1 - par2);
	pr_info("CALC: mul = %ld\n", par1 * par2);
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("HELLO: Goodbye linux kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);
