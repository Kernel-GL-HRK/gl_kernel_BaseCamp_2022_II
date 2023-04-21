// SPDX-License-Identifier: GPL-3.0-or-later

#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel

MODULE_LICENSE("GPL");              					///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Laura Shcherbak");   					///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux kernel module"); 	///< The description -- see modinfo
MODULE_VERSION("1.0");              					///< The version of the module

static int v1;
module_param(v1, int, 0444); 			///< Read permission bit for all users
MODULE_PARM_DESC(v1, "First value");  	///< parameter description

static int v2;
module_param(v2, int, 0444);
MODULE_PARM_DESC(v2, "Second value");

static int __init hello_init(void)
{
	pr_info("HELLO: Hello Kernel\n");
	pr_info("Sum: %d + %d = %d\n", v1, v2, v1+v2);
	pr_info("Substraction: %d - %d = %d\n", v1, v2, v1-v2);
	pr_info("Multiplication: %d * %d = %d\n", v1, v2, v1*v2);
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("HELLO: Goodbye Kernel\n");
}

module_init(hello_init);
module_exit(hello_exit);


