// SPDX-License-Identifier: GPL-2.0
/*
 * 24.05.2023
 *
 * This is a simple program to demonstrate the operation of Linux kernel
 * drivers - display, gyroscope and matrix keyboard.
 * We get the direction of movement from the gyroscope and the 4x4 matrix
 * keyboard. By direction - we move the "head" on the display.
 * To work with the display, we use the frame buffer and memory mapping.
 *
 * Direction:
 * 4x4 matrix keypad:		2 [UP]
 *			4 [LEFT]	6 [RIGHT]
 *				8 [DOWN]
 *
 * TODO: implement the snake game.
 */

#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define	SYS_GYRO	"/sys/devices/mpu6050/direct"
#define DEV_KEYB	"/dev/gl_keyb"

#define RED_OF		(11)
#define GREEN_OF	(5)
#define BLUE_OF		(0)
// RGB565 color code
#define RGB565_CC(RED, GREEN, BLUE)	((RED << RED_OF) \
					 | (GREEN << GREEN_OF) \
					 | (BLUE << BLUE_OF))

// pixel color
#define WHT	RGB565_CC(31, 63, 31)
#define	RED	RGB565_CC(31, 0, 0)
#define GRN	RGB565_CC(0, 63, 0)
#define BLE	RGB565_CC(0, 0, 31)
#define BLK	RGB565_CC(0, 0, 0)

// head 16x16
static const uint16_t head[][16] = {
	{WHT, WHT, BLK, BLK, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, BLK, BLK, WHT, WHT},
	{WHT, WHT, BLK, BLK, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, BLK, BLK, WHT, WHT},
	{BLK, BLK, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, BLK, BLK},
	{BLK, BLK, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, BLK, BLK},
	{WHT, WHT, WHT, WHT, RED, RED, WHT, WHT, WHT, WHT, RED, RED, WHT, WHT, WHT, WHT},
	{WHT, WHT, WHT, WHT, RED, RED, WHT, WHT, WHT, WHT, RED, RED, WHT, WHT, WHT, WHT},
	{WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT},
	{WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT},
	{WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT},
	{WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT},
	{WHT, WHT, WHT, WHT, WHT, RED, WHT, WHT, WHT, WHT, RED, WHT, WHT, WHT, WHT, WHT},
	{BLK, BLK, WHT, WHT, WHT, WHT, RED, RED, RED, RED, WHT, WHT, WHT, WHT, BLK, BLK},
	{BLK, BLK, WHT, WHT, WHT, WHT, RED, RED, RED, RED, WHT, WHT, WHT, WHT, BLK, BLK},
	{BLK, BLK, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, BLK, BLK},
	{BLK, BLK, BLK, BLK, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, BLK, BLK, BLK, BLK},
	{BLK, BLK, BLK, BLK, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, BLK, BLK, BLK, BLK},
};

void print_head(const uint32_t x_pos, const uint32_t y_pos,
		const uint32_t xres, const uint32_t yres, const char *fb)
{
	uint32_t x;
	uint32_t y;

	for (size_t i = 0; i < sizeof(head) / sizeof(head[0]); ++i) {
		for (size_t j = 0; j < sizeof(head[0]) / sizeof(head[0][0]); ++j) {
			x = j + x_pos;
			y = i + y_pos;
			if (x >= xres) {
				x -= xres;
			}

			if (y >= yres) {
				y -= yres;
			}

			*((uint16_t *)(fb + (x + y * xres) * 2)) = head[i][j];
		}
	}
}

void set_background(const uint16_t bg_color,
		    const uint32_t screen_size,
		    const char *fb)
{
	for (size_t i = 0; i < screen_size / 2; ++i) {
		*((uint16_t *)(fb + i * 2)) = bg_color;
	}
}

void delay(uint32_t number_of_seconds)
{
	// Converting time into milli_seconds
	uint32_t milli_seconds = 1000 * number_of_seconds;

	// Storing start time
	clock_t start_time = clock();

	// looping till required time is not achieved
	while (clock() < start_time + milli_seconds)
		;
}

int main(void)
{
	int fbfd;
	struct fb_var_screeninfo vinfo;
	uint32_t screen_size;
	char *fbptr;
	char direct;
	char buf_gy[10];
	char buf_kb[10];
	FILE *fd_gy, *fd_kb;
	uint16_t backgroud_black;
	uint32_t x_coord, y_coord;

	/* The counter used to exit from infinite loop */
	uint32_t temp = 2000;

	/* open framebuffer */
	fbfd = open("/dev/fb0", O_RDWR);
	if (fbfd == -1) {
		perror("Error opening framebuffer device\n");
		return 1;
	}

	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		perror("Error reading variable screen info\n");
		close(fbfd);
		return 2;
	}

	/* memory mapping */
	screen_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	fbptr = (char *)mmap(0, screen_size, PROT_READ | PROT_WRITE,
			     MAP_SHARED, fbfd, 0);

	if (fbptr == MAP_FAILED) {
		perror("Error mapping framebuffer\n");
		close(fbfd);
		return 3;
	}

	/* set background - black */
	backgroud_black = (0 << vinfo.red.offset)
		| (0 << vinfo.green.offset) | (0 << vinfo.blue.offset);

	set_background(backgroud_black, screen_size, fbptr);

	/*
	 * Initial parameters.
	 * Start from the center of the display.
	 * The "head" move to the right.
	 */
	direct = '6';
	x_coord = vinfo.xres / 2;
	y_coord = vinfo.yres / 2;

	while (temp) {
		--temp;

		/* Read the direction from the gyroscope (sysfs) */
		fd_gy = fopen(SYS_GYRO, "r");
		if (fd_gy == NULL) {
			perror("Cannot open mpu6050 device\n");
			goto fail;
		}

		while (fgets(buf_gy, 10, fd_gy)) {
			if (buf_gy[0] != '0') {
				direct = buf_gy[0];
			}
		}
		fclose(fd_gy);

		/* Read the direction from the 4x4 matrix keypad (chardev) */
		fd_kb = fopen(DEV_KEYB, "r");
		if (fd_kb == NULL) {
			perror("Cannot open matrix keypad device\n");
			goto fail;
		}

		while (fgets(buf_kb, 10, fd_kb)) {
			if (buf_kb[0] != 0) {
				direct = buf_kb[0];
			}
		}
		fclose(fd_kb);

		/* change coordinates (x, y) */
		switch (direct) {
		// up
		case '2':
			--y_coord;
			if (y_coord <= 0) {
				y_coord += vinfo.yres;
			}
			break;
		// down
		case '8':
			++y_coord;
			if (y_coord >= vinfo.yres) {
				y_coord -= vinfo.yres;
			}
			break;
		// left
		case '4':
			--x_coord;
			if (x_coord <= 0) {
				x_coord += vinfo.xres;
			}
			break;
		// right
		case '6':
			++x_coord;
			if (x_coord >= vinfo.xres) {
				x_coord -= vinfo.xres;
			}
		default:
			break;
		}

		print_head(x_coord, y_coord, vinfo.xres, vinfo.yres, fbptr);
		delay(50);
		set_background(backgroud_black, screen_size, fbptr);
	}

	close(fbfd);
	return 0;

fail:
	exit(EXIT_FAILURE);
}

