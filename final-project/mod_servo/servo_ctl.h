#include <linux/pwm.h>

#define FREQ_FOR_SERVO		20000000
#define MAX_DUTY_CYCLE		2500000
#define MIN_DUTY_CYCLE		500000
#define MIN_DELAY_FOR_DEG	2
#define MAX_ANGLE			170
#define MIN_ANGLE			0

extern struct pwm_device *servo;
extern uint32_t currentAngle;

int32_t servo_set_angle_abs(uint32_t angle);
int32_t servo_set_angle_rel(int32_t angle);