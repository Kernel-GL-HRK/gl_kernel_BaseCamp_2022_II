// SPDX-License-Identifier: GPL
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kushnir Vladyslav");
MODULE_DESCRIPTION("A simple module");
MODULE_VERSION("0.1");

static int first;
static int second;

module_param(first, int, 0000);
MODULE_PARM_DESC(first, "First namber.");
module_param(second, int, 0000);
MODULE_PARM_DESC(second, "Second namber.");


static int __init init(void)
{
	pr_info("The 'first_module' module starts.");
	pr_info("\tThe sum of %d and %d numbers is equal %d.\n", first, second, first + second);
	pr_info("\tThe difference of %d and %d is equal %d.\n", first, second, first - second);
	pr_info("\tThe product of %d and %d is equal %d.\n", first, second, first * second);

	return 0;
}

static void __exit exit_module(void)
{
	pr_info("The work of the 'first_module' module is completed.\n");
}

module_init(init);
module_exit(exit_module);
