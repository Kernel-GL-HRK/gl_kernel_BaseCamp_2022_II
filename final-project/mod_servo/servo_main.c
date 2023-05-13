#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/err.h>
#include <linux/mod_devicetable.h>
#include <linux/of_device.h>
#include "sys_dev.h"
#include "proc.h"
#include "platform_driver.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muravka Roman");
MODULE_DESCRIPTION("Driver for servo with devFS, procFS, Device Tree");
MODULE_VERSION("0.1");

static int servo_init(void)
{
	int32_t err = 0;
	{//Device Tree
		pr_info("servo: Creating platform driver file system\n");
		err = create_platform_driver();
		if (err)
			goto dt_err;
	}
	{//DEV
		pr_info("servo: Creating /dev file system\n");
		err = create_devFS();
		if (err)
			goto dev_err;
	}
	{//PROC
		pr_info("servo: Creating /proc file system\n");
		err = create_procFS();
		if (err)
			goto proc_err;
	}
	return 0;

proc_err:
	remove_devFS();
dev_err:
	remove_platform_driver();
dt_err:
	return err;
}

static void servo_exit(void)
{
	pr_info("servo: Removing /dev file system\n");
	remove_devFS();
	pr_info("servo: Removing /proc file system\n");
	remove_procFS();
	pr_info("servo: Removing platform driver file system\n");
	remove_platform_driver();
}

module_init(servo_init);
module_exit(servo_exit);
