// SPDX-License-Identifier: GPL-2.0

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init/h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Viacheslav Us");
MODULE_DESCRIPTION("First hello kernel module");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
	pr_info("Hello World!\n");

	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Goodbye World!\n");
}

module_init(hello_init);
module_exit(hello_exit);


