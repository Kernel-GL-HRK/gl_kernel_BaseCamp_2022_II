#include <linux/proc_fs.h>
#include <linux/types.h>

#define PROC_MAX_BUFFER_SIZE	1024
#define PROC_FILE_NAME 			"FP-ultrasound"
#define ONLY_READ_MODE 			0444

#define FIRST_MESSAGE	0
#define US_TRIG			1
#define US_ECHO			2

#define INTRODUCTION	"------------------------------------------------------------------------------------------------------------------\n"\
						"- The ultrasonic sensor works by sending a 40 kHz sound trigger and after bouncing off an obstacle,\n"\
						"echo receives this signal. Distance is calculated by calculating the time between sending and receiving a signal.\n"\
						"- Max distance - 4m\n"\
						"- Viewing angle - 15 degrees\n"\
						"- Time between uses - 60 ms"\
						"- Driver for ultrasound has two parameters:\n"\
						"    1) gpio number for trigger - for sending sending a 40 kHz sound\n"\
						"    2) gpio number for echo - for receiving signal\n"\
						"------------------------------------------------------------------------------------------------------------------\n"\


extern struct proc_dir_entry *proc_file;
extern struct proc_ops proc_fops;

int32_t create_procFS(void);
void remove_procFS(void);