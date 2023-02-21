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

static int subst[2];
static int subst_count;
module_param_array(subst, int, &subst_count, 0);
MODULE_PARM_DESC(subst, "Substraction of two integers");

static int prod[2];
static int prod_count;
module_param_array(prod, int, &prod_count, 0);
MODULE_PARM_DESC(prod, "Substraction of two integers");

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

	if (subst_count) {
		if (subst_count == 2) {
			result = subst[0] - subst[1];
			pr_info("SUBST(%d,%d) = %d\n", subst[0], subst[1], result);
		} else {
			pr_err("SUBST passed %d parameters, must be 2!\n",
			       subst_count);
			return -EINVAL;
		}
	}

	if (prod_count) {
		if (prod_count == 2) {
			result = prod[0] * prod[1];
			pr_info("PROD(%d,%d) = %d\n", prod[0], prod[1], result);
		} else {
			pr_err("PRODT passed %d parameters, must be 2!\n",
			       prod_count);
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
