// SPDX-License-Identifier: GPL
/*
 * Simple kernel module,
 * which receive two params
 * and make some arithmetics
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vlad Degtyarov <deesyync@gmail.com>");
MODULE_DESCRIPTION("Simple kernel module");
MODULE_VERSION("0.1");

static s32 p1, p2;

module_param(p1, int, 0644);
module_param(p2, int, 0644);

static s32 __init hellau_init(void)
{
	s32 sum = p1 + p2;
	s32 diff = p1 - p2;
	s32 prod = p1 * p2;

	pr_notice(KBUILD_MODNAME ": Initializing kernel module...\n");

	pr_info("Parameter \"p1\" = %d\n", p1);
	pr_info("Parameter \"p2\" = %d\n", p2);

	if (p1 == 0 && p2 == 0) {
		pr_warn("There will be no magic if you won\'t set params!\n");
		goto at_last;
	}

	pr_info("The sum of %d and %d is %d\n", p1, p2, sum);
	pr_info("The diff of %d and %d is %d\n", p1, p2, diff);
	pr_info("The product of %d and %d is %d\n", p1, p2, prod);

	return 0;

at_last:
	pr_notice(KBUILD_MODNAME ": Module was not loaded.\n");
	return -EINVAL;
}

static void __exit hellau_exit(void)
{
	pr_notice(KBUILD_MODNAME ": Exiting kernel module.\n");
}

module_init(hellau_init);
module_exit(hellau_exit);
