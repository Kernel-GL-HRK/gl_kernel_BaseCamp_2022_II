#include <linux/of_device.h>
#include <linux/platform_device.h>

#define NAME_OF_DT_NODE	"servo"
#define PR_CHANNEL		"pwm-channel"
#define PR_MODE			"mode"

struct servo_params {
	uint32_t pwm_channel;
	char *mode;
	uint32_t angle;
	int8_t status[256];
};

int32_t create_platform_driver(void);
void remove_platform_driver(void);

struct servo_params get_servo_description(void);