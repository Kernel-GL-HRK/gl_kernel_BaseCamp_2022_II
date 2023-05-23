#include "controlling_servo.h"
#include <fcntl.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER 1024

static FILE *dev_file;

struct servo_description check_description_servo(void) {
	char buffer[MAX_BUFFER] = {0};
	struct servo_description description = {0,0,{0},0}; // инициализируем все поля нулями

	FILE *proc_file = fopen(SERVO_PROC, "r");
	if (proc_file == NULL) {
		perror("can not open proc file of servo\n");
		description.error = -1;
		return description;
	}
	if (fseek(proc_file, CHANNEL_SERVO, SEEK_SET) != 0) {
		perror("fseek error\n");
		description.error = -1;
		fclose(proc_file);
		return description;
	}
	fread(buffer, sizeof(char), MAX_BUFFER-1, proc_file);

	for (int32_t i = 1, j = 0; i < MAX_PARAMS_SERVO; i++) {
		int16_t need_copy = -1;

		while (buffer[j] != ']') {
		switch (i) {
		case CHANNEL_SERVO:
			if ((buffer[j] >= '0') && (buffer[j] <= '9'))
				description.pwm_channel = (description.pwm_channel * 10) + (buffer[j] - '0');
			break;
		case SPEED_SERVO:
			if ((buffer[j] >= '0') && (buffer[j] <= '9'))
				description.speed = (description.speed * 10) + (buffer[j] - '0');
			break;
		case MODE_SERVO:
			if (need_copy >= 0)
				description.mode[need_copy++] = buffer[j];
			if (buffer[j] == '[')
				need_copy++;
			break;
		}
		buffer[j++] = ' ';
		}
		buffer[j++] = ' ';
	}
	printf("channel of servo: %d\n", description.pwm_channel);
	printf("speed of servo: %d\n", description.speed);
	printf("mode of servo: %s\n", description.mode);



	fclose(proc_file);
	return description;
}

int32_t open_dev_file_servo(void)
{
    dev_file = fopen(SERVO_DEV, "r+");
    if (dev_file == NULL) {
        perror("Error opening servo_control");
        return -1;
    }
	return 0;
}
int32_t close_dev_file_servo(void)
{
	fclose(dev_file);
}

int32_t turn_servo(int32_t angle)
{
	char buffer[MAX_BUFFER] = {0};
	
	if (angle > MAX_ANGLE)
		angle = MAX_ANGLE;
	sprintf(buffer, "%d", angle);
	
	fwrite(buffer, sizeof(char), strlen(buffer), dev_file);
	if (ferror(dev_file)) {
		perror("Cannot write to dev file");
		return -1;
	}

	return 0;
}

int32_t get_angle_servo(void)
{
	char buffer[MAX_BUFFER] = {0};
	uint32_t angle, i;

	fread(buffer, sizeof(char), MAX_BUFFER, dev_file);
	if (ferror(dev_file)) {
		perror("Cannot read to dev file");
		return -1;
	}

	for (i = 0; buffer[i] != ']'; i++) {
		if ((buffer[i] >= '0') && (buffer[i] <= '9'))
			angle = (angle * 10) + (buffer[i] - '0');
	}
	
	return 0;
}