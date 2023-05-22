
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include "st7735.h"


#define ST7735_TFTWIDTH	160
#define ST7735_TFTHEIGHT 128

#define MY_BLOCK_SIZE (1024*4*4)

static uint8_t block_data[MY_BLOCK_SIZE];
static uint16_t block_index;

static void send_block_data(struct spi_device *spi)
{

	spi_write(spi, block_data, block_index);

	block_index = 0;
}



static void add_data_to_block(uint16_t data, struct spi_device *spi)
{

	block_data[block_index++] = data >> 8;
	block_data[block_index++] = data & 0xFF;


	if (block_index >= MY_BLOCK_SIZE)
		send_block_data(spi);

}



static void spi_command(struct spi_device *spi, unsigned char c)
{
	DC_LOW;

	spi_write(spi, (u8 *)&c, 1);
	DC_HIGH;
}

static void spi_data(struct spi_device *spi, unsigned char c)
{
	DC_HIGH;

	spi_write(spi, (u8 *)&c, 1);
	DC_LOW;
}

void write_word(uint16_t word, struct spi_device *spi)
{
	spi_data(spi, (word >> 8));
	spi_data(spi, (word & 0xFF));
}

void set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, struct spi_device *spi)
{
	spi_command(spi, ST7735_CASET);
	write_word(x0, spi);
	write_word(x1, spi);

	spi_command(spi, ST7735_RASET);
	write_word(y0, spi);
	write_word(y1, spi);

	spi_command(spi, ST7735_RAMWR);
}


void clear_display(uint16_t color, struct spi_device *spi)
{
	uint16_t i;
	uint16_t pixels = ST7735_TFTWIDTH * ST7735_TFTHEIGHT;

	set_addr_window(0, 0, ST7735_TFTHEIGHT - 1, ST7735_TFTWIDTH - 1, spi);

	for (i = 0; i < pixels; ++i)
		add_data_to_block(color, spi);

	send_block_data(spi);
}


unsigned char *color_show(unsigned char *color_name, uint16_t color_value)
{
	switch (color_value) {
	case BLACK:
			strcpy(color_name, "BLACK");
			break;
	case WHITE:
			strcpy(color_name, "WHITE");
			break;
	case RED:
			strcpy(color_name, "RED");
			break;
	case GREEN:
			strcpy(color_name, "GREEN");
			break;
	case BLUE:
			strcpy(color_name, "BLUE");
			break;
	case YELLOW:
			strcpy(color_name, "YELLOW");
			break;
	case MAGENTA:
			strcpy(color_name, "MAGENTA");
			break;
	case CYAN:
			strcpy(color_name, "CYAN");
			break;
	case GRAY:
			strcpy(color_name, "GRAY");
			break;
	case SILVER:
			strcpy(color_name, "SILVER");
			break;
	case GOLD:
			strcpy(color_name, "GOLD");
			break;
	default:
			strcpy(color_name, "UNKNOWN");
			break;
	}

	return color_name;
}



void ST7735_DrawChar(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size, struct spi_device *spi)
{
	uint8_t line;// horizontal row of pixels of character
	int32_t col, row, i, j;// loop indices

	if ((x + 6 * size - 1) >= ST7735_TFTHEIGHT || // Clip right
		(y + 8 * size - 1) >= ST7735_TFTWIDTH) { // Clip bottom
		return;
	}

	set_addr_window(x, y, x + 6 * size - 1, y + 8 * size - 1, spi);

	line = 0x01;		// print the top row first
// print the rows, starting at the top
	for (row = 0; row < 8; row++) {
	for (i = 0; i < size; i++) {
		uint8_t fontIndex[5];

		for (col = 0; col < 5; col++)
			fontIndex[col] = Font[(uint8_t)c * 5 + col];

		for (col = 0; col < 5; col++) {
			uint8_t pixelData = fontIndex[col];
			for (j = 0; j < size; j++) {
				if (pixelData & line) {
			// bit is set in Font, add pixel(s) in text color to block data
					add_data_to_block(textColor, spi);
				} else {
// bit is cleared in Font, add pixel(s) in background color to block data
					add_data_to_block(bgColor, spi);
			}
		}
		}
		// add blank column(s) to the right of character to block data
		for (j = 0; j < size; j++)
			add_data_to_block(bgColor, spi);
	}
	line <<= 1;	// move up to the next row
	}

	send_block_data(spi);
}


uint32_t ST7735_DrawString(char *pt, size_t buffer_size, struct display_font *font, struct spi_device *spi)
{
	uint16_t count = 0;
	uint16_t x = font->x;
	uint32_t y = font->y;

	if (y > 15)
		return 0;
	while (*pt && (++count < buffer_size)) {
		if ((*pt == '\n') || ((*pt == '\\') && (*(++pt) == 'n'))) {
			if (*(++pt) && (++count < buffer_size))
				x = 20;
			else
				return count;
		}

		if (x >= 20) {
			x = font->x;
			y += font->font_size;
			if (y > 15)
				return count;
		}

		ST7735_DrawChar(x * 6, y * 10, *pt, font->text_color, font->bg_color, font->font_size, spi);
		pt++;
		x += font->font_size;
	}
	return count;
}



int _init_display(struct spi_device *spi)
{
	mdelay(100);

	RST_LOW;
	udelay(20);
	RST_HIGH;
	mdelay(120);

	spi_command(spi, ST7735_SWRESET);// software reset
	mdelay(150);

	spi_command(spi, ST7735_SLPOUT);		// out of sleep mode
	mdelay(120);

	spi_command(spi, ST7735_COLMOD);		// set color mode
	spi_data(spi, 0x05);			// 16-bit color

	spi_command(spi, ST7735_FRMCTR1);// frame rate control
	spi_data(spi, 0x00);			// fastest refresh
	spi_data(spi, 0x06);			// 6 lines front porch
	spi_data(spi, 0x03);			// 3 lines backporch

	spi_command(spi, ST7735_MADCTL);
#define _BV(x) (1 << x)
	spi_data(spi, _BV(3));

	spi_command(spi, ST7735_DISSET5);// display settings #5
	spi_data(spi, 0x15);			// 1 clock cycle nonoverlap, 2 cycle gate rise, 3 cycle oscil. equalize
	spi_data(spi, 0x02);			// fix on VTL

	spi_command(spi, ST7735_PWCTR1);		// power control
	spi_data(spi, 0x02);			// GVDD = 4.7V
	spi_data(spi, 0x70);
			// 1.0uA
	spi_command(spi, ST7735_PWCTR2);		// power control
	spi_data(spi, 0x05);			// VGH = 14.7V, VGL = -7.35V
	spi_data(spi, ST7735_PWCTR3);		// power control
	spi_data(spi, 0x01);			// Opamp current small
	spi_data(spi, 0x02);			// Boost frequency

	spi_command(spi, ST7735_VMCTR1);		// power control
	spi_data(spi, 0x3C);			// VCOMH = 4V
	spi_data(spi, 0x38);			// VCOML = -1.1V

	spi_command(spi, ST7735_PWCTR6);		// power control
	spi_data(spi, 0x11);
	spi_data(spi, 0x15);

	spi_command(spi, ST7735_GMCTRP1);

	spi_data(spi, 0x09);
	spi_data(spi, 0x16);
	spi_data(spi, 0x09);
	spi_data(spi, 0x20);
	spi_data(spi, 0x21);
	spi_data(spi, 0x1B);
	spi_data(spi, 0x13);
	spi_data(spi, 0x19);
	spi_data(spi, 0x17);
	spi_data(spi, 0x15);
	spi_data(spi, 0x1E);
	spi_data(spi, 0x2B);
	spi_data(spi, 0x04);
	spi_data(spi, 0x05);
	spi_data(spi, 0x02);
	spi_data(spi, 0x0E);

	spi_command(spi, ST7735_GMCTRN1);
	spi_data(spi, 0x0B);
	spi_data(spi, 0x14);
	spi_data(spi, 0x08);
	spi_data(spi, 0x1E);
	spi_data(spi, 0x22);
	spi_data(spi, 0x1D);
	spi_data(spi, 0x18);
	spi_data(spi, 0x1E);
	spi_data(spi, 0x1B);
	spi_data(spi, 0x1A);
	spi_data(spi, 0x24);
	spi_data(spi, 0x2B);
	mdelay(120);
	spi_data(spi, 0x3f);
	mdelay(10);
	spi_command(spi, ST7735_DISPON);
	mdelay(120);

	return 0;
}
