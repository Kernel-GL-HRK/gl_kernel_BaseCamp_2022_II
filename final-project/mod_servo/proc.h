#include <linux/proc_fs.h>
#include <linux/types.h>

#define PROC_MAX_BUFFER_SIZE 1024

#define PROC_FILE_NAME "FP-servo"
#define ONLY_READ_MODE 0444

#define FIRST_MESSAGE	0
#define SERVO_CHANNEL	1
#define SERVO_MODE		2

#define INTRODUCTION	"------------------------------------------------------------------------------------------------------------------\n"\
						"- Servomotor work by PWM(frequency: 20ms; duty-cycle: 0.5ms>*<2.5ms).\n"\
						"- Max angle - 170 degrees\n"\
						"- Driver for servomotor has two parameters:\n"\
						"    1) pwm channel - channel through which a PWM signal will be sent to work with a servo\n"\
						"    2) servo mode:\n"\
						"        2.1) absolute - the servo will scroll to the specified degree regardless of the previous location\n"\
						"        2.2) relative - the servo will scroll the specified number of degrees depending on the previous location\n"\
						"------------------------------------------------------------------------------------------------------------------\n"\

int32_t create_procFS(void);
void remove_procFS(void);