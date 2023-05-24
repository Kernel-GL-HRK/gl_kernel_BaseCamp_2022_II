#include <stdio.h>
#include <stdint.h>

#define ULTRASONIC_DEV	"/dev/ultrasound_control"
#define ULTRASONIC_PROC	"/proc/FP-ultrasound"
#define MAX_BUFFER	1024
#define ULTRASONIC_ECHO_PIN	"-ECHO_US-"
#define ULTRASONIC_TRIG_PIN "-TRIG_US-"

enum {
	ECHO = 1,
	TRIG,
	MAX_PARAMS_ULTRASONIC
};

struct ultrasonic_description {
	int32_t trig_pin;
	int32_t echo_pin;
	int32_t error;
};

struct ultrasonic_description check_description_ultrasonic(void);

int32_t get_distance_ultrasonic(void);

int32_t open_dev_file_ultrasonic(void);
int32_t close_dev_file_ultrasonic(void);