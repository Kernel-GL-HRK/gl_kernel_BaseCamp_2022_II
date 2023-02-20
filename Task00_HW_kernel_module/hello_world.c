// SPDX-License-Identifier: GPL-2.0+

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Yurii Bezkor");
MODULE_DESCRIPTION("Hello world kernel module");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
	pr_info("HELLO: Hello Kernel!\n");
	return 0;
}
static void __exit hello_exit(void)
{
	pr_info("HELLO: Goodbye Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);
