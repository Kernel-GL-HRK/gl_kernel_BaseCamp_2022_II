#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/property.h>
#include <linux/err.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/pwm.h>
#include "platform_driver.h"
#include "servo_ctl.h"

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
struct pwm_device *servo;
static struct servo_params servo_device = {.status = "disabled"};

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
	uint32_t error;

	{//Reading pwm channel for servo from device tree
		error = device_property_present(node_of_servo, PR_CHANNEL);
		if (!error) {
			pr_err("servo-dt-property: %s property is not present\n", PR_CHANNEL);
			return -error;
		}

		error = device_property_read_u32(node_of_servo, PR_CHANNEL, &servo_device.pwm_channel);
		if (error) {
			pr_err("servo-dt-property: %s property can not read\n", PR_CHANNEL);
			return -error;
		}

		pr_info("servo-dt-property: pwm channel for servo: %d\n", servo_device.pwm_channel);
	}
	{//Reading servo mode from device tree
		error = device_property_present(node_of_servo, PR_MODE);
		if (!error) {
			pr_err("servo-dt-property: %s property is not present\n", PR_MODE);
			return -error;
		}

		error = device_property_read_string(node_of_servo, PR_MODE, (const char **)&servo_device.mode);
		if (error) {
			pr_err("servo-dt-property: %s property can not read\n", PR_MODE);
			return -error;
		}

		pr_info("servo-dt-property: pwm mode for servo: %s\n", servo_device.mode);
	}
	{//Requesting pwm channel for servo
		servo = pwm_request(servo_device.pwm_channel, "servo-module");
		if (IS_ERR(servo)) {
			pr_err("servo-dt-pwm: can not request pwm channel #%d", servo_device.pwm_channel);
			return PTR_ERR(servo);
		}
		strcpy(servo_device.status, "enabled");
	}
	{//Test
		uint32_t i;
		
		for (i = 0; i < 5; i++) {
			servo_set_angle_abs(MAX_ANGLE);
			servo_set_angle_abs(MIN_ANGLE);
		}
	}
	return 0;
}

int dt_remove(struct platform_device *devp)
{
	servo_set_angle_abs(0);
	pwm_free(servo);
	strcpy(servo_device.status, "disabled");
	return 0;
}

struct servo_params get_servo_description(void)
{
	return servo_device;
}
