// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
#include "pr_fmt.h"
#include <linux/init.h>
#include <linux/module.h>
#include "gpio.h"
#include "proc.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module that uses procfs ang gpio");
MODULE_AUTHOR("Tereshchenko Dmytro <buxdmo@gmail.com>");
MODULE_VERSION("1.0");

/*===============================================================================================*/
static int driver_init(void)
{
	pr_info("module init");

	if (gpio_init(27)) {
		pr_err("Failed to init gpio\n");
		return -EFAULT;
	}

	if (proc_init()) {
		pr_err("Failed to init proc\n");
		gpio_exit();
		return -EFAULT;
	}

	return 0;
}
/*===============================================================================================*/
static void driver_exit(void)
{
	pr_info("module exit");
	gpio_exit();
	proc_exit();
}
/*===============================================================================================*/

module_init(driver_init);
module_exit(driver_exit);
