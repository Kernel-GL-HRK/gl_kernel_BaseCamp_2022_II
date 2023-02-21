// SPDX-License-Identifier: GPL-2.0+

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Yurii Bezkor");
MODULE_DESCRIPTION("Hello world kernel module");
MODULE_VERSION("0.2");

static int sum[2];
static int sum_count;
module_param_array(sum, int, &sum_count, 0);
MODULE_PARM_DESC(sum, "Sum of two integers");

static int __init hello_init(void)
{
	int result;

	pr_info("HELLO: Hello Kernel!\n");
	if (sum_count) {
		if (sum_count == 2) {
			result = sum[0] + sum[1];
			pr_info("SUM(%d,%d) = %d\n", sum[0], sum[1], result);
		} else {
			pr_err("SUM passed %d parameters, must be 2!\n",
			       sum_count);
			return -EINVAL;
		}
	}

	return 0;
}
static void __exit hello_exit(void)
{
	pr_info("HELLO: Goodbye Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);
