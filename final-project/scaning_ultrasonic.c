#include "scaning_ultrasonic.h"
#include <fcntl.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER 1024

static int32_t dev_file;

struct ultrasonic_description check_description_ultrasonic(void)
{
	struct ultrasonic_description description = {0, 0, 0};

	int32_t proc_file = open(ULTRASONIC_PROC, O_RDONLY);

	if (proc_file < 0) {
		perror("can not open proc file of ultrasonic\n");
		description.error = -1;
		return description;
	}

	for (int32_t i = 1; i < MAX_PARAMS_ULTRASONIC; i++) {
		int32_t temp = 0;
		int16_t need_copy = -1;
		char buffer[MAX_BUFFER] = {0};

		if (lseek(proc_file, i, SEEK_SET) < 0) {
			perror("lseek error\n");
			description.error = -1;
			close(proc_file);
			return description;
		}
		read(proc_file, buffer, MAX_BUFFER);

		for (int j = 0; buffer[j] != '\0'; j++) {
			if ((buffer[j] >= '0') && (buffer[j] <= '9'))
				temp = (temp * 10) + (buffer[j] - '0');

			switch (i) {
			case ECHO:
				description.echo_pin = temp;
				break;
			case TRIG:
				description.trig_pin = temp;
				break;
		}
		}
	}

	close(proc_file);
	return description;
}

int32_t open_dev_file_ultrasonic(void)
{
    dev_file = open(ULTRASONIC_DEV, O_RDONLY);
    if (dev_file < 0) {
		perror("Error opening ultrasonic_control");
		return -1;
    }
	return 0;
}
int32_t close_dev_file_ultrasonic(void)
{
	close(dev_file);
}

int32_t get_distance_ultrasonic(void)
{
	char buffer[MAX_BUFFER] = {0};
	uint32_t distance = 0, i;

	if (read(dev_file, buffer, MAX_BUFFER) < 0) {
		perror("Cannot read from dev file");
		return -1;
	}

	for (i = 0; buffer[i] != ']'; i++) {
		if ((buffer[i] >= '0') && (buffer[i] <= '9'))
			distance = (distance * 10) + (buffer[i] - '0');
	}

	return distance;
}