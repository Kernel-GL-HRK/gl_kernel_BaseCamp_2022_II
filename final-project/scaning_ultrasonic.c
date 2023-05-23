#include "scaning_ultrasonic.h"
#include <fcntl.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER 1024

static FILE *dev_file;

struct ultrasonic_description check_description_ultrasonic(void) {
	char buffer[MAX_BUFFER] = {0};
	struct ultrasonic_description description = {0,0,0}; // инициализируем все поля нулями

	FILE *proc_file = fopen(ULTRASONIC_PROC, "r");
	if (proc_file == NULL) {
		perror("can not open proc file of ultrasonic\n");
		description.error = -1;
		return description;
	}
	if (fseek(proc_file, ECHO, SEEK_SET) != 0) {
		perror("fseek error\n");
		description.error = -1;
		fclose(proc_file);
		return description;
	}
	fread(buffer, sizeof(char), MAX_BUFFER-1, proc_file);

	for (int32_t i = 1, j = 0; i < MAX_PARAMS_ULTRASONIC; i++) {
		int16_t need_copy = -1, temp = 0;

		while (buffer[j] != ']') {
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
		buffer[j++] = ' ';
		}
		buffer[j++] = ' ';
	}
	printf("echo pin of ultrasonic: %d\n", description.echo_pin);
	printf("trigger pin of ultrasonic: %d\n", description.trig_pin);

	fclose(proc_file);
	return description;
}

int32_t open_dev_file_ultrasonic(void)
{
    dev_file = fopen(ULTRASONIC_DEV, "r");
    if (dev_file == NULL) {
        perror("Error opening ultrasonic_control");
        return -1;
    }
	return 0;
}
int32_t close_dev_file_ultrasonic(void)
{
	fclose(dev_file);
}

int32_t get_distance_ultrasonic(void)
{
	char buffer[MAX_BUFFER] = {0};
	uint32_t distance = 0, i;

	fread(buffer, sizeof(char), 257, dev_file);
	if (ferror(dev_file)) {
		perror("Cannot read from dev file");
		return -1;
	}

	printf("%s \n", buffer);

	for (i = 0; buffer[i] != ']'; i++) {
		if ((buffer[i] >= '0') && (buffer[i] <= '9'))
			distance = (distance * 10) + (buffer[i] - '0');
	}
	printf("gi: %d\n", distance);

	return 0;
}