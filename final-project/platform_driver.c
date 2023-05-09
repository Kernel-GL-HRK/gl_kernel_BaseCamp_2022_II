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
	const char *servo_mode;
	uint32_t error;

	{//Reading pwm channel for servo from device tree
		error = device_property_present(node_of_servo, PR_CHANNEL);
		if (!error) {
			pr_err("servo-dt-property: %s property is not present\n", PR_CHANNEL);
			return -error;
		}

		error = device_property_read_u32(node_of_servo, PR_CHANNEL, &servo_pwm_channel);
		if (error) {
			pr_err("servo-dt-property: %s property can not read\n", PR_CHANNEL);
			return -error;
		}

		pr_info("servo-dt-property: pwm channel for servo: %d\n", servo_pwm_channel);
	}
	{//Reading servo mode from device tree
		error = device_property_present(node_of_servo, PR_MODE);
		if (!error) {
			pr_err("servo-dt-property: %s property is not present\n", PR_MODE);
			return -error;
		}

		error = device_property_read_string(node_of_servo, PR_MODE, &servo_mode);
		if (error) {
			pr_err("servo-dt-property: %s property can not read\n", PR_MODE);
			return -error;
		}

		pr_info("servo-dt-property: pwm mode for servo: %s\n", servo_mode);
	}
	{//Requesting pwm channel for servo
		servo = pwm_request(servo_pwm_channel, "servo-module");
		if(IS_ERR(servo)) {
			pr_err("servo-dt-pwm: can not request pwm channel #%d", servo_pwm_channel);
			return PTR_ERR(servo);
		}
	}

	if(strcmp(servo_mode, "absolute") == 0) {
		servo_set_angle_abs(180);
		msleep(1000);
		servo_set_angle_abs(125);
		msleep(1000);
		servo_set_angle_abs(130);
		msleep(1000);
		servo_set_angle_abs(25);
		msleep(1000);
		servo_set_angle_abs(75);
		msleep(1000);
		servo_set_angle_abs(0);
		msleep(1000);
	}
	if(strcmp(servo_mode, "relative") == 0) {
		servo_set_angle_rel(190);
		msleep(1000);
		servo_set_angle_rel(1);
		msleep(1000);
		servo_set_angle_rel(-50);
		msleep(1000);
		servo_set_angle_rel(25);
		msleep(1000);
		servo_set_angle_rel(-100);
		msleep(1000);
		servo_set_angle_rel(-233);
		msleep(1000);
		servo_set_angle_rel(0);
		msleep(1000);
	}
	return 0;
}

int dt_remove(struct platform_device *devp)
{
	uint32_t i;
	{
		for(i = 0; i < 20; i++)
			if(currentAngle == 0) {
				servo_set_angle_abs(170);
			}
			else 
				servo_set_angle_abs(0);
	}
	servo_set_angle_abs(0);

	pwm_free(servo);
	return 0;
}