// SPDX-License-Identifier: GPL 2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>


#define MODULE_NAME module_name(THIS_MODULE)

static int parm1, parm2;

module_param(parm1, int, 0644);
MODULE_PARM_DESC(parm1, "first number");
module_param(parm2, int, 0644);
MODULE_PARM_DESC(parm2, "second number");


static int __init calculator_init(void)
{

	if ((!parm1) || (!parm2)) {
		pr_err("%s: Numbers must be not equal 0. Exiting.\n\n", MODULE_NAME);
		return -EINVAL;
	}

	pr_err("%s: First number is: %d\n", MODULE_NAME, parm1);
	pr_err("%s: Second number is: %d\n\n", MODULE_NAME, parm2);
	pr_err("%s: %d addition %d is equal: %d\n", MODULE_NAME, parm1, parm2, parm1+parm2);
	pr_err("%s: %d substraction %d is equal: %d\n", MODULE_NAME, parm1, parm2, parm1-parm2);
	pr_err("%s: %d multiplication %d is equal: %d\n", MODULE_NAME, parm1, parm2, parm1*parm2);
	pr_err("%s: %d modulus %d is equal: %d\n", MODULE_NAME, parm1, parm2, parm1%parm2);

	return 0;
}

static void __exit calculator_exit(void)
{
	pr_err("%s: is unloaded\n", MODULE_NAME);
}

module_init(calculator_init);
module_exit(calculator_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytro Volkov");
MODULE_DESCRIPTION("A simple demo of passing parameters to kernel module.");
MODULE_VERSION("0.01");
