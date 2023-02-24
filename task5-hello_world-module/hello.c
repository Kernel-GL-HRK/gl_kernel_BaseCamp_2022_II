// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
/*
 * Kernel module
 * During init it prints info message "Hello..."
 * Calculates sum, subtraction and product of two parameters
 * During exit it prints info message "Goodbye..."
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vadym Minziuk");
MODULE_DESCRIPTION("Hello from Kernel Module");
MODULE_VERSION("0.1");

static int num1 = 1;
module_param(num1, int, 0664);

static int num2 = -1;
module_param(num2, int, 0664);


static int __init hello_init(void)
{
	pr_info("HELLO: Hello from Kernel Module!\n");
	pr_info("HELLO: sum = %d\n", 0 + num1 + num2);
	pr_info("HELLO: subtraction = %d\n", 0 + num1 - num2);
	pr_info("HELLO: product = %d\n", 0 + num1 * num2);
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("HELLO: Goodbye from Kernel Module!\n");
}

module_init(hello_init);
module_exit(hello_exit);
