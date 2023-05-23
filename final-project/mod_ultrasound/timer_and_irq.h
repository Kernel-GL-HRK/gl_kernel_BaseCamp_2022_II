#include <linux/types.h>
#include <linux/interrupt.h>

#define	NAME_OF_INTERRUPT_IRQ		"interrupt_for_ultrasound"
#define	PERIOD_FOR_ULTRASOUND_MSEC	60
#define ON	1
#define OFF	0


int32_t run_ultrasound(void);
void stop_ultrasound(void);

uint32_t get_distance(void);
