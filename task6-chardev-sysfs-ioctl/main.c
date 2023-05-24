#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "led_driver.h"

int main(void) 
{
	int32_t delay, r_delay;
	int fd = open(DEV_FILE, O_RDWR);

	if (fd < 0) {
		perror("open");
		exit(1);
	}

	printf("Device: " DEV_FILE "\n");
	printf("Enter delay (in ms): ");
	scanf("%d", &delay);

	printf("Writing to driver...\n");
	if (ioctl(fd, LED_WR_DELAY, &delay) < 0) {
		perror("ioctl");
		exit(1);
	}

	printf("Reading from driver...\n");
	if (ioctl(fd, LED_RD_DELAY, &r_delay) < 0) {
		perror("ioctl");
		exit(1);
	}
	printf("Delay is %d ms\n", r_delay);

	/* Testing unappropriate behavoir */
	printf("\nDoing with driver something not permitted...\n");
	if (ioctl(fd, 100, &r_delay) < 0) {
		perror("ioctl");
		exit(1);
	}

	close(fd);
	return 0;
}
