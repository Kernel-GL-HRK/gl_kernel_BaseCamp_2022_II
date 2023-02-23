// SPDX-License-Identifier: GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergiy Us");
MODULE_DESCRIPTION("Hello kernel module");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
	pr_info("HELLO: Hello linux kernel!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("HELLO: Goodbye linux kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);
