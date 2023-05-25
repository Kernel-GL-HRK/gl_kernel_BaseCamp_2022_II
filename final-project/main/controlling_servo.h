#include <stdio.h>
#include <stdint.h>

#define SERVO_DEV	"/dev/servo_control"
#define SERVO_PROC	"/proc/FP-servo"
#define MAX_BUFFER	1024

#define SERVO_PIN_NAME		"SERVO_PWM"
#define SERVO_PIN_CHANNEL	18

#define MAX_ANGLE	170
#define MIN_ANGLE	0
enum {
	CHANNEL_SERVO = 1,
	MODE_SERVO,
	SPEED_SERVO,
	MAX_PARAMS_SERVO
};

struct servo_description {
	int32_t pwm_channel;
	int32_t speed;
	int8_t	mode[32];
	int32_t error;
};

struct servo_description check_description_servo(void);

int32_t turn_servo(int32_t angle);
int32_t get_angle_servo(void);

int32_t open_dev_file_servo(void);
int32_t close_dev_file_servo(void);