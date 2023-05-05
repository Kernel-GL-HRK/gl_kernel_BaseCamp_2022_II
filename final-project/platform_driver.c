#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/property.h>
#include <linux/err.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include "platform_driver.h"

MODULE_LICENSE("GPL");
MODULE_DEVICE_TABLE(of, dt_id);

struct of_device_id dt_id[] = {
	{.name = NAME_OF_DT_NODE},
	{}
};
struct platform_driver dev_servo = {
	.probe = dt_probe,
	.remove = dt_remove,
	.driver = {
		.name = "servo_driver",
		.of_match_table = dt_id
	},
};

int32_t create_platform_driver(void)
{
	int32_t err;

	err = platform_driver_register(&dev_servo);
	if (err) {
		pr_err("servo-dt: driver can not register\n");
		return -err;
	}
	pr_info("servo-dt: driver is registered\n");

	return 0;
}

void remove_platform_driver(void)
{
	platform_driver_unregister(&dev_servo);
	pr_info("servo-dt: driver is unregistered\n");
}

int dt_probe(struct platform_device *devp)
{
	struct device *node_of_servo = &devp->dev;
	uint32_t servo_pwm_channel;
	uint8_t *servo_mode;

	device_property_read_u32(node_of_servo, PR_CHANNEL, &servo_pwm_channel);
	pr_info("servo-dt-property: pwm channel for servo: %d\n", servo_pwm_channel);

	device_property_read_string(node_of_servo, PR_MODE, (const char **)&servo_mode);
	pr_info("servo-dt-property: pwm mode for servo: %s\n", servo_mode);
	kfree(servo_mode);
	return 0;
}

int dt_remove(struct platform_device *devp)
{
	return 0;
}