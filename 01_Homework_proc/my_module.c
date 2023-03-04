// SPDX-License-Identifier: GPL-2.0+

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Yurii Bezkor");
MODULE_DESCRIPTION("Proc module");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
	pr_info("PROC module: Hello Kernel!\n");
	return 0;
}
static void __exit hello_exit(void)
{
	pr_info("PROC module: Goodbye Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);
