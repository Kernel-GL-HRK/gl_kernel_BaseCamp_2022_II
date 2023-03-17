// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Yurii Bezkor");
MODULE_DESCRIPTION("Hello world kernel module");
MODULE_VERSION("0.1");

static int __init gbtp_init(void)
{
	pr_info("GBTP: Hello Kernel!\n");
	return 0;
}
static void __exit gbtp_exit(void)
{
	pr_info("GBTP: Goodbye Kernel!\n");
}

module_init(gbtp_init);
module_exit(gbtp_exit);
