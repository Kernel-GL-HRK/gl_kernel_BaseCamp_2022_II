// SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
/*
 * Userspace program for kernel module
 * Character device: matrix keypad 4x4
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEV_KEYB	"/dev/chrdev0"
#define READ_CYCLES	100

void delay(int msec)
{
	// Convert to milli_seconds
	int mseconds = 1000 * msec;

	// Stor start time
	clock_t start_time = clock();

	// loop while stop time is not achieved
	while (clock() < start_time + mseconds)
	;
}

int main(void)
{
	FILE *fdk;
	char buf_k[1];
	char oldbuf = '\0';

	for (size_t i = 0; i < READ_CYCLES; ++i) {

		// open file and read
		fdk = fopen(DEV_KEYB, "r");
		if (fdk == NULL) {
			perror("Cannot open keypad dev\n");
			goto fail;
		}

		fgets(buf_k, 2, fdk);

		fclose(fdk);

		if (oldbuf != buf_k[0]) {
			if (oldbuf != '\0') {
//				printf("key: %c\n", buf_k[0]);
				printf("%c\n", buf_k[0]);
			}
			oldbuf = buf_k[0];
		}

		// delay
		delay(500);
	}

	return 0;

fail:
	exit(EXIT_FAILURE);
}
