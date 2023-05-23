#include "controlling_servo.h"
#include <fcntl.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER 1024

static int32_t dev_file;

struct servo_description check_description_servo(void)
{
	struct servo_description description = {0, 0, {0}, 0}; // инициализируем все поля нулями

	int32_t proc_file = open(SERVO_PROC, O_RDONLY);
	if (proc_file < 0) {
		perror("can not open proc file of servo\n");
		description.error = -1;
		return description;
	}

	for (int32_t i = 1; i < MAX_PARAMS_SERVO; i++) {
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

		for (int j = 0; buffer[j] != ']'; j++) {
			if ((buffer[j] >= '0') && (buffer[j] <= '9'))
				temp = (temp * 10) + (buffer[j] - '0');

			switch (i) {
			case CHANNEL_SERVO:
				description.pwm_channel = temp;
				break;
			case SPEED_SERVO:
				description.speed = temp;
				break;
			case MODE_SERVO:
				if (need_copy >= 0)
					description.mode[need_copy++] = buffer[j];
				if (buffer[j] == '[')
					need_copy++;
				break;
			}
		}
	}
	close(proc_file);
	return description;
}

int32_t open_dev_file_servo(void)
{
	dev_file = open(SERVO_DEV, O_RDWR);
	if (dev_file < 0) {
		perror("Error opening servo_control");
		return -1;
	}
	return 0;
}
int32_t close_dev_file_servo(void)
{
	close(dev_file);
}

int32_t turn_servo(int32_t angle)
{
	char buffer[MAX_BUFFER] = {0};

	if (angle > MAX_ANGLE)
		angle = MAX_ANGLE;
	sprintf(buffer, "%d", angle);

	if (write(dev_file, buffer, strlen(buffer)) < 0)
		perror("cannot write to servo\n");

	return 0;
}

int32_t get_angle_servo(void)
{
	char buffer[MAX_BUFFER] = {0};
	uint32_t angle = 0, i;

	if (read(dev_file, buffer, MAX_BUFFER) < 0) {
		perror("Cannot read to dev file");
		return -1;
	}

	for (i = 0; buffer[i] != ']'; i++) {
		if ((buffer[i] >= '0') && (buffer[i] <= '9'))
			angle = (angle * 10) + (buffer[i] - '0');
	}
	return angle;
}