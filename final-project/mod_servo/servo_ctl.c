#include <linux/module.h>
#include <linux/pwm.h>
#include <linux/printk.h>
#include <linux/err.h>
#include <linux/delay.h>
#include "linux/sched.h"
#include "servo_ctl.h"
#include "platform_driver.h"


MODULE_LICENSE("GPL");

uint32_t currentAngle = 0;

static void servo_turn(uint32_t angle)
{
	uint64_t converted;
	uint32_t delay;
	struct servo_params servo_description;

	servo_description = get_servo_description();

	delay = (servo_description.speed < MIN_DELAY_FOR_DEG) ? MIN_DELAY_FOR_DEG : servo_description.speed;

	//converted = (((MAX_DUTY_CYCLE - MIN_DUTY_CYCLE) / 170) * angle) + MIN_DUTY_CYCLE;
	//pwm_config(servo, converted, FREQ_FOR_SERVO);
	//pwm_enable(servo);

	while (currentAngle != angle) {
		pwm_disable(servo);

		converted = (((MAX_DUTY_CYCLE - MIN_DUTY_CYCLE) / 170) * currentAngle) + MIN_DUTY_CYCLE;
		pwm_config(servo, converted, FREQ_FOR_SERVO);

		pwm_enable(servo);

		msleep(delay);

		if (currentAngle > angle) {
			currentAngle--;
		} else {
			currentAngle++;
		}
	}
	pwm_disable(servo);
	converted = (((MAX_DUTY_CYCLE - MIN_DUTY_CYCLE) / 170) * currentAngle) + MIN_DUTY_CYCLE;
	pwm_config(servo, converted, FREQ_FOR_SERVO);
	pwm_enable(servo);

	msleep(delay);
}

int32_t servo_set_angle_abs(uint32_t angle)
{
	if (angle > MAX_ANGLE) {
		pr_warn("servo-ctl: angle more than 170 degrees. servomotor will rotate to 170 degrees\n");
		angle = MAX_ANGLE;
	}
	servo_turn(angle);

	pr_info("servo-ctl: servomotor rotaded %d\n", currentAngle);

	return currentAngle;
}

int32_t servo_set_angle_rel(int32_t angle)
{
	
	int32_t rel_angle = angle;

	if (angle > (int32_t)(MAX_ANGLE - currentAngle)) {
		pr_warn("servo-ctl: angle more than 170 degrees. servomotot will rotate to 170 degrees\n");
		angle = MAX_ANGLE;
		rel_angle = MAX_ANGLE - currentAngle;
	} else if ((int32_t)(angle + currentAngle) < 0) {
		pr_warn("servo-ctl: angle more than 0 degrees. servomotot will rotate to 0 degrees\n");
		angle = MIN_ANGLE;
		rel_angle = MIN_ANGLE - currentAngle;
	} else
		angle = angle + currentAngle;
	servo_turn(angle);
	
	pr_info("servo-ctl: servomotor rotaded %d degrees(%d)\n", rel_angle, currentAngle);

	return currentAngle;
}