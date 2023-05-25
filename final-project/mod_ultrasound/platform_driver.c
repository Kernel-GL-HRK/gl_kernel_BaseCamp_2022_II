#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/property.h>
#include <linux/err.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include "asm-generic/gpio.h"
#include "platform_driver.h"
#include "timer_and_irq.h"

MODULE_LICENSE("GPL");

int dt_probe(struct platform_device *devp);
int dt_remove(struct platform_device *devp);

static struct ultrasound_desc ultrasound = {.status = "disconnected"};

struct of_device_id dt_id[] = {
	{.name = NAME_OF_DT_NODE},
	{}
};
MODULE_DEVICE_TABLE(of, dt_id);

struct platform_driver dev_ultrasound = {
	.probe = dt_probe,
	.remove = dt_remove,
	.driver = {
		.name = "ultrasound_driver",
		.of_match_table = dt_id
	},
};

int32_t create_platform_driver(void)
{
	int32_t err;

	err = platform_driver_register(&dev_ultrasound);
	if (err) {
		pr_err("ultrasound-dt: driver can not register\n");
		return err;
	}
	pr_info("ultrasound-dt: driver is registered\n");

	return 0;
}

void remove_platform_driver(void)
{
	platform_driver_unregister(&dev_ultrasound);
	pr_info("ultrasound-dt: driver is unregistered\n");
}

int dt_probe(struct platform_device *devp)
{
	struct device *node_of_ultrasound = &devp->dev;
	uint32_t error;

	{//Reading number of echo pin for ultrasound from device tree
		uint32_t ultra_echo;

		error = device_property_present(node_of_ultrasound, PR_ECHO);
		if (!error) {
			pr_err("ultrasound-dt-property: %s property is not present\n", PR_ECHO);
			return -EINVAL;
		}

		device_property_read_u32(node_of_ultrasound, PR_ECHO, &ultra_echo);
		pr_info("ultrasound-dt-property: number of echo pin for ultrasound: %d\n", ultra_echo);
		ultrasound.echo_pin = ultra_echo;
	}
	{//Reading number of trigger pin for ultrasound from device tree
		uint32_t ultra_trig;

		error = device_property_present(node_of_ultrasound, PR_TRIG);
		if (!error) {
			pr_err("ultrasound-dt-property: %s property is not present\n", PR_TRIG);
			return -EINVAL;
		}

		device_property_read_u32(node_of_ultrasound, PR_TRIG, &ultra_trig);
		pr_info("ultrasound-dt-property: number of trigger pin for ultrasound: %d\n", ultra_trig);
		ultrasound.trig_pin = ultra_trig;
	}
	{//Requesting pins for ultrasound driver
		error = gpio_request(ultrasound.trig_pin, "trigger pin for ultrasound\n");
		if (error < 0) {
			pr_err("ultrasound-dt-gpio: can not request trigger pin #%d\n", ultrasound.trig_pin);
			return error;
		}
		gpio_direction_output(ultrasound.trig_pin, 0);

		error = gpio_request(ultrasound.echo_pin, "echo pin for ultrasound\n");
		if (error < 0) {
			pr_err("ultrasound-dt-gpio: can not request echo pin #%d\n", ultrasound.echo_pin);
			return error;
		}
		gpio_direction_input(ultrasound.echo_pin);

		pr_info("ultrasound-dt-gpio: pins (echo: %d; trigger: %d) are requested\n", ultrasound.echo_pin, ultrasound.trig_pin);		

		sprintf(ultrasound.status,  "connected");
	}

	run_ultrasound();
	return 0;
}

int dt_remove(struct platform_device *devp)
{
	stop_ultrasound();
	gpio_free(ultrasound.echo_pin);
	gpio_free(ultrasound.trig_pin);
	sprintf(ultrasound.status,  "disconnected");
	
	pr_info("ultrasound-dt: device tree is uploaded, gpios are free\n");
	return 0;
}

struct ultrasound_desc get_ultra_description (void)
{
	return ultrasound;
}