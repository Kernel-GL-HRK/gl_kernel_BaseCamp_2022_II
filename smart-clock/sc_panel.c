// SPDX-License-Identifier: GPL-2.0-only
/*
 * Module library: sc_panel
 * Description: st7735fb display driver and display functions for the module
 * Module: smart-clock
 *
 * Copyright (C) 2023 Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/ktime.h>
#include <linux/of_gpio.h>

#include "include/project1.h"
#include "include/panel.h"
#include "include/draw.h"

/* Screen params */
#define BPP		(16)
#define BPP_24		(24)
#define BPP_32		(32)

#define MEM_SIZE	(WIDTH*HEIGHT*BPP/8)
#define DEVICE_NAME	"st7735fb"


/* bmp header offsets and values */
#define BMP_HEADER_IMAGE_START_OFF	0x0a
#define BMP_HEADER_SIZE_OFF		0x0e
#define BMP_HEADER_WIDTH_OFF		0x12
#define BMP_HEADER_HEIGHT_OFF		0x16
#define BMP_HEADER_BPP_OFF		0x1c

#define BMP_IMAGE_START		0x42
#define BMP_IMAGE_HEADER_SIZE		0x28


uint8_t timer_blink_bmask, alarm_blink_bmask;
static void display_update_work(struct work_struct *);


/*-----------------------------------------------------------*/
/*	st7735 driver functions	START			     */
/*-----------------------------------------------------------*/


/**
 * external draw_icon() - drawing icon to vmem
 * @icon: enum icon from icons.h
 * @x: coordinate by x-axis
 * @y: coordinate by y-axis
 * @color: enum color displayed
 */
void draw_icon(uint8_t icon, uint16_t x, uint16_t y, uint16_t color)
{
	uint16_t *vmem16 = (uint16_t *)st7735fb.screen_base;
	uint8_t xc, yc;

	uint16_t icon_offset = icon * icons.height *
	 (icons.width / 8 + (icons.width % 8 ? 1 : 0));

	uint8_t *ptr = &icons.table[icon_offset];

	for (yc = 0; yc < icons.height; yc++) {
		for (xc = 0; xc < icons.width; xc++) {
			if (*ptr & (0x80 >> (xc % 8)))
				vmem16[(y+yc)*WIDTH+(x+xc)] = color;
			if (xc % 8 == 7)
				ptr++;
		}
		if (icons.width % 8 != 0)
			ptr++;
	}
}

/**
 * extern st7735fb_draw_rectangle() - drawing rectangle or line to vmem
 * @xs: start coordinate by x-axis
 * @ys: start coordinate by y-axis
 * @xe: end coordinate by x-axis
 * @ye: end coordinate by y-axis
 * @color: enum color displayed
 * @filled: if true will be filled
 */
void st7735fb_draw_rectangle(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color, uint8_t filled)
{
	uint16_t *vmem16 = (uint16_t *)st7735fb.screen_base;
	uint16_t x = 0, y = 0;

	for (x = 0; x < WIDTH; x++)
		for (y = 0; y < HEIGHT; y++) {
			if ((((x == xs) || (x == xe)) && (y >= ys) && (y <= ye)) ||
				 (((y == ys) || (y == ye)) && (x >= xs) && (x <= xe)))
				vmem16[(y)*WIDTH+(x)] = color;
			if (filled)
				if (((x >= xs) && (x <= xe)) && ((y >= ys) && (y <= ye)))
					vmem16[(y)*WIDTH+(x)] = color;
			}
}


/**
 * external st7735fb_draw_string() - drawing string to vmem
 * @x: start coordinate by x-axis
 * @y: start coordinate by y-axis
 * @font: bitmap font
 * @x_offset: x bias between chars in text
 * @color: enum color displayed
 */
void st7735fb_draw_string(char *text, uint16_t x, uint16_t y, struct Bitmap *font, uint8_t x_offset, uint16_t color)
{
	uint8_t char_x, char_y, i = 0;

	/* attach vmem */
	uint16_t *vmem16 = (uint16_t *)st7735fb.screen_base;

	while (text[i] != 0) {
		/* find char offset */
		uint16_t char_offset = (text[i] - ' ') * font->height *
		 (font->width / 8 + (font->width % 8 ? 1 : 0));
		uint8_t *ptr = &font->table[char_offset];
		/* get display char */
		for (char_y = 0; char_y < font->height; char_y++) {
			for (char_x = 0; char_x < font->width; char_x++) {
				if (*ptr & (0x80 >> (char_x % 8)))
					/* write vmem */
					vmem16[(y+char_y)*WIDTH+(x+char_x)] = color;
				if (char_x % 8 == 7)
					ptr++;
			}
			if (font->width % 8 != 0)
				ptr++;
		}
		x += x_offset;
		i++;
	}
}

/**
 * st7735fb_blank_vmem() - erase display vmem
 *
 * external, used when to erase vmem
 */
void st7735fb_blank_vmem(void)
{
	memset(st7735fb.screen_base,  0, MEM_SIZE);
}


/**
 * external int st7735fb_send_buff_display() - display bmp file from device sysfs
 *
 * returns 0 on success
 *
 */
int st7735fb_send_buff_display(void)
{
	uint16_t y, x, xs = 0, ys = 0, bmp_size_x, bmp_size_y, header;
	uint16_t *vmem16 = (uint16_t *)st7735fb.screen_base;

	/* Clear display buffer */
	st7735fb_blank_vmem();
	/* Read bmp header and exit if size is bigger than our screen */
	bmp_size_x = fs_buffer.buf[BMP_HEADER_WIDTH_OFF];
	bmp_size_y = fs_buffer.buf[BMP_HEADER_HEIGHT_OFF];
	header = fs_buffer.buf[BMP_HEADER_IMAGE_START_OFF]+1;
	if ((bmp_size_x > WIDTH) || (bmp_size_y > HEIGHT)) {
		st7735fb_draw_string("Not supported size", 5, 50, &font[FONT16], 8, RED_COLOR);
		return 1;
	}
	/* Re-Center image if its smaller than screen size */
	if (bmp_size_x < WIDTH)
		xs = (WIDTH-bmp_size_x)/2;
	if (bmp_size_y < HEIGHT)
		ys = (HEIGHT-bmp_size_y)/2;

	for (y = bmp_size_y; y > 0; y--) {
		while (x < bmp_size_x) {
			/* if bmp is 16 bit color use 2 bytes color pallete */
			if (fs_buffer.buf[BMP_HEADER_BPP_OFF] == BPP)
				vmem16[(y+ys)*WIDTH+(x+xs)] = (fs_buffer.buf[header++] << 8) |
								(fs_buffer.buf[header++]);
			/* if bmp is 24 bit color use 3 bytes color pallete */
			if (fs_buffer.buf[BMP_HEADER_BPP_OFF] == BPP_24)
				vmem16[(y+ys)*WIDTH+(x+xs)] = (fs_buffer.buf[header++] << 16) |
								(fs_buffer.buf[header++] << 8) |
								(fs_buffer.buf[header++]);
			/* if bmp is 32 bit color use 4 bytes color pallete */
			if (fs_buffer.buf[BMP_HEADER_BPP_OFF] == BPP_32)
				vmem16[(y+ys)*WIDTH+(x+xs)] = (fs_buffer.buf[header++] << 24) |
								(fs_buffer.buf[header++] << 16) |
								(fs_buffer.buf[header++] << 8) |
								(fs_buffer.buf[header++]);
		x++;
		}
		x = 0;
	}

	return 0;
}

/**
 * st7735fb_draw_buff_display() - reads display memory and copys to buffer in bmp format
 *
 * external, called from @device_read_write()
 *
 */
int st7735fb_get_buff_display(void)
{
	int x, y; 
	uint32_t header;

	uint16_t *vmem16 = (uint16_t *)st7735fb.screen_base;

	fs_buffer.buf =  kzalloc(MEM_SIZE+BMP_IMAGE_START, GFP_KERNEL);

	if (fs_buffer.buf == NULL) {
		pr_err("%s: malloc for buffer failed", __func__);
		return -ENOMEM;
	}
	/* Storing bmp header */
	sprintf(fs_buffer.buf, "%s", "BM"); //BM at start

	fs_buffer.buf[BMP_HEADER_WIDTH_OFF] = WIDTH;
	fs_buffer.buf[BMP_HEADER_HEIGHT_OFF] = HEIGHT;
	fs_buffer.buf[BMP_HEADER_IMAGE_START_OFF] = BMP_IMAGE_START;
	fs_buffer.buf[BMP_HEADER_BPP_OFF] = BPP;
	fs_buffer.buf[BMP_HEADER_SIZE_OFF] = BMP_IMAGE_HEADER_SIZE;
	header = BMP_IMAGE_START;
	for (y = HEIGHT; y > 0; y--)
		for (x = 0; x <  WIDTH; x++) {
			fs_buffer.buf[header++] = (uint8_t)((vmem16[(y)*WIDTH+(x)])&0xff);
			fs_buffer.buf[header++] = (uint8_t)((vmem16[(y)*WIDTH+(x)])>>8);
		}
	/* Add filesize to header */
	fs_buffer.buf_len = header-1;
	memcpy(fs_buffer.buf+2, &fs_buffer.buf_len, sizeof(uint32_t));
	return 0;
}

/**
 * st7735fb_notifications_overlay() - add notifications to display view
 *
 * called in @st7735fb_update_display to add actual alarm & timer notifiations to screen view
 *
 */
static void st7735fb_notifications_overlay(void)
{
	if ((options.is_alarm_enabled) && (my_button.view_mode != GAME))
		draw_icon(0, 0, 0, DISP_NOTIF_COLOR);
	if ((our_timer.nsec) && (my_button.view_mode != TIMER) && (my_button.view_mode != GAME)) {
		timer_blink_bmask ^= 1 << 1;
		draw_icon(1, 25, 0, ((timer_blink_bmask >> 1) & 1) ? DISP_NOTIF_COLOR : 0);
	} else
		timer_blink_bmask = 0xFF;
}

/**
 * st7735fb_alarm_overlay() - add alarm to display view
 *
 * called in @st7735fb_update_display when alarm is triggered will display alarm message in any display views
 *
 */
static void st7735fb_alarm_overlay(void)
{
	if ((clock_and_alarm.is_alarm) && (my_button.view_mode != ALARM)) {
		alarm_blink_bmask ^= 1 << 1;
		if  ((alarm_blink_bmask >> 1) & 1)
			st7735fb_draw_string("ALARM!!!", 0, 50, &font[FONT32], 20, DISP_NOTIF_COLOR);
	} else
		alarm_blink_bmask = 0xFF; // clear blinking state
}


/*-----------------------------------------------------------*/
/*	st7735 driver functions	END			     */
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/*	st7735 display driver  START			     */
/*-----------------------------------------------------------*/


/**
 *  st7735_write_data() - writes data to spi.
 *
 *  returns non-zero on fail
 */
static int st7735_write_data(const uint8_t *data, size_t size)
{
	/* Set data mode */
	gpio_set_value(st7735fb.dc, 1);

	/* Write entire buffer */
	return spi_write(st7735fb.spi, data, size);
}

/**
 *  st7735_write_cmd() - write display CMD (1 byte).
 *
 *  returns non-zero on fail
 */
static void st7735_write_cmd(uint8_t data)
{
	int ret = 0;

	/* Set command mode */
	gpio_set_value(st7735fb.dc, 0);

	ret = spi_write(st7735fb.spi, &data, 1);
	if (ret < 0)
		pr_err("%s: write data failed\n", DEVICE_NAME);

}

/**
 *  st7735_run_cfg() - run display init configuarion script.
 */
static void st7735_run_cfg(void)
{
	uint8_t i = 0;
	uint8_t end_script = 0;

	do {
		switch (st7735_cfg[i].cmd) {
		case	ST7735_START:
				break;
		case ST7735_CMD:
				st7735_write_cmd(st7735_cfg[i].data);
				break;
		case ST7735_DATA:
				st7735_write_data(&st7735_cfg[i].data, 1);
				break;
		case ST7735_DELAY:
				mdelay(st7735_cfg[i].data);
				break;
		case ST7735_END:
				end_script = 1;
		}
		i++;
	} while (!end_script);
}

/**
 *  st7735_reset()
 */
static void st7735_reset(void)
{
	/* Reset controller */
	gpio_set_value(st7735fb.rst, 0);
	udelay(10);
	gpio_set_value(st7735fb.rst, 1);
	mdelay(120);
}

/**
 * display_update_work() - write data to disp spi workqueue
 *
 */
static void display_update_work(struct work_struct *work)
{
	int ret = 0;
	uint8_t *vmem = st7735fb.screen_base;
	int i;
	uint16_t *vmem16;
	uint16_t *smem16;
	unsigned int write_nbytes;

	vmem16 = (uint16_t *)vmem;
	smem16 = (uint16_t *)st7735fb.spi_writebuf;
	for (i = 0; i < HEIGHT; i++) {
		int x;

		for (x = 0; x < WIDTH; x++)
			smem16[x] = (vmem16[x]>>8)|(vmem16[x]<<8);
		smem16 += WIDTH*BPP/16;
		vmem16 += WIDTH*BPP/16;
	}

	/* RAM write command */
	st7735_write_cmd(ST7735_RAMWR);

	/* spi_writebuf write */
	write_nbytes = WIDTH*HEIGHT*BPP/8;
	ret = st7735_write_data(st7735fb.spi_writebuf, write_nbytes);
	if (ret < 0)
		pr_err("%s: write data failed\n", DEVICE_NAME);

}

DECLARE_WORK(update_diplay, display_update_work);

/**
 * external st7735fb_update_display() - schedule @display_update_work()
 *
 * update screen view
 */
void st7735fb_update_display(void)
{

	/*add notification overlay*/
	st7735fb_notifications_overlay();
	/*add  alarm overlay*/
	st7735fb_alarm_overlay();
	schedule_work(&update_diplay);
}

/**
 * st7735fb_init_display() - init display from probe
 *
 */
static int st7735fb_init_display(void)
{

	if (spi_w8r8(st7735fb.spi, ST7735_NOP) < 0)
		return -ENODEV;
	st7735_reset();
	st7735_run_cfg();
	st7735fb_update_display();
	return 0;
}
/**
 * ds3231_probe() - probe and allocate memory for display and spi data
 *
 */
static int st7735fb_probe(struct spi_device *spi)
{
	struct device_node *np = spi->dev.of_node;
	uint8_t status = 0;
	uint8_t *vmem = NULL;
	uint8_t *spi_writebuf = NULL;
	int retval = -EINVAL;


	pr_err("%s: probe started\n", DEVICE_NAME);


	spi->mode = SPI_MODE_0;
	status = spi_setup(spi);

	if (status < 0) {
		dev_err(&spi->dev, "%s: SPI setup error %d\n",
			__func__, status);
		return status;
	}

	dev_err(&spi->dev, "SPI ok\n");

	st7735fb.spi = spi;
	st7735fb.dc = of_get_named_gpio(np, "dc-gpios", 0);
	st7735fb.rst = of_get_named_gpio(np, "reset-gpios", 0);

	/* Request GPIOs and initialize to default values */
	if (gpio_request_one(st7735fb.rst, GPIOF_OUT_INIT_HIGH, "ST7735 Reset Pin")) {
		pr_err("%s: Cannot request GPIO\n", DEVICE_NAME);
		retval = -EBUSY;
		goto gpio_fail;
	}
	if (gpio_request_one(st7735fb.dc, GPIOF_OUT_INIT_LOW, "ST7735 Data/Command Pin")) {
		pr_err("%s: Cannot request GPIO\n", DEVICE_NAME);
		retval = -EBUSY;
		goto gpio_fail;
	}


	vmem = kzalloc(MEM_SIZE, GFP_KERNEL);
	if (!vmem) {
		retval = -ENOMEM;
		goto alloc_fail;
		}
	st7735fb.screen_base = (uint8_t __force __iomem *)vmem;
	pr_err("%s: Total vm initialized %d\n", DEVICE_NAME, MEM_SIZE);

	/* Allocate spi write buffer */
	spi_writebuf = kzalloc(MEM_SIZE, GFP_KERNEL);
	if (!spi_writebuf) {
		retval = -ENOMEM;
		goto alloc_fail;
		}
	st7735fb.spi_writebuf = spi_writebuf;

	retval = st7735fb_init_display();

	/*reporting probe to init */
	init_hw.st7735_spi_probed = 1;

	return retval;

alloc_fail:
	if (spi_writebuf)
		kfree(spi_writebuf);
	if (vmem)
		kfree(vmem);
gpio_fail:
	gpio_free(st7735fb.dc);
	gpio_free(st7735fb.rst);
	return retval;
};


static const struct of_device_id st7735fb_of_match_table[] = {
	{
		.compatible = "smart-clock,st7735fb",
	},
	{},
};
MODULE_DEVICE_TABLE(of, st7735fb_of_match_table);




struct spi_driver st7735fb_driver = {
	.driver = {
		.name   = "st7735fb",
		.owner  = THIS_MODULE,
		.of_match_table = st7735fb_of_match_table,
	},
	.probe  = st7735fb_probe,
};


/**
 *  st7735fb_init() - init module library from main.
 *
 */
int st7735fb_init(void)
{
	pr_err("%s: init\n", DEVICE_NAME);
	return spi_register_driver(&st7735fb_driver);
}

/**
 *  st7735fb_unload() - exit module library from main.
 *
 */
void st7735fb_unload(void)
{
	pr_err("%s: exiting\n", DEVICE_NAME);
	spi_unregister_driver(&st7735fb_driver);
	msleep(100);
	if (st7735fb.screen_base)
		kfree(st7735fb.screen_base);
	if (st7735fb.spi_writebuf)
		kfree(st7735fb.spi_writebuf);
	gpio_free(st7735fb.dc);
	gpio_free(st7735fb.rst);
	pr_err("%s: unloaded\n", DEVICE_NAME);

}
/*-----------------------------------------------------------*/
/*	st7735 display driver  END			     */
/*-----------------------------------------------------------*/


