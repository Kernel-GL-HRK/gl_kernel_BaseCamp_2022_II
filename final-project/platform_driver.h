#include "linux/types.h"
#include <linux/of_device.h>
#include <linux/platform_device.h>

#define NAME_OF_DT_NODE	"ultrasound"
#define PR_ECHO			"echo_pin"
#define PR_TRIG			"trig_pin"

struct ultrasound_desc {
	uint32_t echo_pin;
	uint32_t trig_pin;
	int8_t status[32];
};

int32_t create_platform_driver(void);
void remove_platform_driver(void);

struct ultrasound_desc get_ultra_description (void);


