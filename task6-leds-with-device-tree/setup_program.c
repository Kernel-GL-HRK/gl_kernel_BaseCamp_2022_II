#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ioctl-cmd.h"

#define EXIT 5

int main()
{
	int32_t file_desc;
	uint32_t ch;
	uint64_t msec_rgb = 1000;
	uint32_t command;

	file_desc = open(PATH_TO_IOCTL, O_RDWR);
	if (file_desc < 0) {
		printf("file is not opened\n");
		return -1;
	}
	
	do {
	printf("Menu of choice led:\n");
	printf("1 - red;\n2 - green\n3 - blue\n4 - switch off all leds;\n5 - exit.\n");
	printf("Please enter value for choice: ");
	scanf("%d", &ch);
	switch (ch) {
	case RED:
		command = _IOW(CH_DEV, RED, sizeof(msec_rgb));
		printf("Please enter value for time of work led: ");
		scanf("%ld", &msec_rgb);
		break;
	case GREEN:
		command = _IOW(CH_DEV, GREEN, sizeof(msec_rgb));
		printf("Please enter value for time of work led: ");
		scanf("%ld", &msec_rgb);
		break;
	case BLUE:
		command = _IOW(CH_DEV, BLUE, sizeof(msec_rgb));
		printf("Please enter value for time of work led: ");
		scanf("%ld", &msec_rgb);
		break;
	case LEDS_OFF:
		command = _IOW(CH_DEV, LEDS_OFF, sizeof(msec_rgb));
		break;
	}
	ioctl(file_desc, command, &msec_rgb);
	} while (ch != EXIT);

	close(file_desc);
	return  0;
}
