// SPDX-License-Identifier: GPL

#define DEBUG

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vlad Degtyarov <deesyync@gmail.com>");
MODULE_DESCRIPTION("Simple kernel module");
MODULE_VERSION("0.2");

#undef pr_fmt
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

static s32 p1;
module_param(p1, int, 0444);
MODULE_PARM_DESC(p1, "First parameter");

static s32 p2;
module_param(p2, int, 0444);
MODULE_PARM_DESC(p2, "Second parameter");

static int __init hellau_init(void)
{
	s32 sum = p1 + p2;
	s32 diff = p1 - p2;
	s32 prod = p1 * p2;

	pr_notice("Initializing kernel module...\n");

	pr_debug("p1 = %d\n", p1);
	pr_debug("p2 = %d\n", p2);

	if (p1 == 0 && p2 == 0) {
		pr_warn("There will be no magic if you won\'t set params!\n");
		goto at_last;
	}

	pr_info("The sum of %d and %d is %d\n", p1, p2, sum);
	pr_info("The diff of %d and %d is %d\n", p1, p2, diff);
	pr_info("The product of %d and %d is %d\n", p1, p2, prod);

	return 0;

at_last:
	pr_notice("Module was not loaded.\n");
	return -EINVAL;
}

static void __exit hellau_exit(void)
{
	pr_notice("Exiting kernel module.\n");
}

module_init(hellau_init);
module_exit(hellau_exit);

#undef DEBUG
