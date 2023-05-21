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
MODULE_DESCRIPTION("Driver for ultrasound with devFS, procFS, Device Tree");
MODULE_VERSION("0.1");

static int ultrasound_init(void)
{
	int32_t err = 0;
	{//DEV
		pr_info("ultrasound: Creating /dev file system\n");
		err = create_devFS();
		if(err)
			goto dev_err;
	}
	{//PROC
		pr_info("ultrasound: Creating /proc file system\n");
		err = create_procFS();
		if(err)
			goto proc_err;
	}
	return 0;

proc_err:
	remove_devFS();
dev_err:
	return err;
}

static void ultrasound_exit(void) 
{
	pr_info("ultrasound: Removing /dev file system\n");
	remove_devFS();
	pr_info("ultrasound: Removing /proc file system\n");
	remove_procFS();
}

module_init(ultrasound_init);
module_exit(ultrasound_exit);
