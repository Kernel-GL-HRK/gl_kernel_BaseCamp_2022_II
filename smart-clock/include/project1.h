/* SPDX-License-Identifier: GPL-2.0-only
 *
 * smart-clock device module header
 *
 *
 * Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#ifndef __PROJECT1_H_INCLUDED
#define __PROJECT1_H_INCLUDED

#include <linux/gpio.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/delay.h>


/* Define our printk format */
#define MODULE_NAME module_name(THIS_MODULE)
#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) "%s: " fmt, KBUILD_MODNAME

/* Define screen shared values */
#define WIDTH		(160)
#define HEIGHT		(128)

/* Game */
#define SNAKE_TAIL_MAX	(1000)


/**
 * enum disp_modes  - display view order by switching
 * @CLOCK: clock view
 * @TIMER: timer view
 * @ALARM: alarm view
 * @TEMP_AND_PRESS: temperature and pressure view
 * @PEDOMETER: pedometer view
 * @GAME: game view
 * @OPTIONS: options view
 */
enum disp_modes {
	CLOCK = 1,
	TIMER,
	ALARM,
	TEMP_AND_PRESS,
	PEDOMETER,
	GAME,
	OPTIONS,
	DISP_MODES_END,
};

/**
 * struct init_hw  - for reporting to device if hardware is connected and working at probe stage
 * @st7735_spi_probed: display driver init
 * @bmp280_i2c_probed: bmp280 sensor init
 * @ds3231_i2c_probed: ds3231 RTC init
 * @mpu6050_i2c_probed: mpu6050 sensor init
 *
 * probe is called when device tree is attached, but hardware may not me physically connected or working at the stage.
 */
struct init_hw {
	uint8_t st7735_spi_probed:1;
	uint8_t bmp280_i2c_probed:1;
	uint8_t ds3231_i2c_probed:1;
	uint8_t mpu6050_i2c_probed:1;
};
extern struct init_hw init_hw;

/**
 * struct Bitmap  - struct used to display fonts and icons on screen
 * @table: pointer to table
 * @width: width in bits of the object
 * @height: height in bits of the object
 *
 */
struct Bitmap {
	uint8_t *table;
	uint8_t width;
	uint8_t height;
};


/**
 * struct button - hardware button states and modes
 * @state: there are three states of presed button
 *	   1 - short press
 *	   2 - long press x1
 *	   3 - long press x2
 * @is_longpress: long press detection for button driver
 * @view_mode:	current view mode
 * @edit_mode:  edit mode used by clock, alarm, options views
 */

struct button {
	uint8_t state;
	uint8_t is_longpress;
	uint8_t view_mode;
	uint8_t edit_mode;

};
extern struct button my_button;


/**
 * struct clock_timer - hardware button states and modes
 * @nsec: current timer value in nsec
 * @nsec_old_ktime: stored kernel time in nsec. Used to keep difference in time while timer is running in background
 * @nsec_stored: stored timer value in nsec.
 * @is_timer_on: timer running flag.
 */
struct clock_timer {
	uint64_t nsec;
	uint64_t nsec_old_ktime;
	uint64_t nsec_stored;
	uint8_t  is_timer_on;
};
extern struct clock_timer our_timer;

/**
 * struct clock - used to keep time and alarm values.Working with rtc dc3231.
 * @clock_sec: current time in sec
 * @clock_stored_sec: keeps time value when clock is in edit mode
 * @alarm_sec: alarm time value in sec.
 * @alarm_stored_sec: keeps alarm time value when alarm is in edit mode.
 * @is_alarm: true when alarm triggered.
 */
struct clock_and_alarm {
	uint64_t clock_sec;
	uint64_t clock_stored_sec;
	uint32_t alarm_sec;
	uint32_t alarm_stored_sec;
	uint8_t is_alarm;
};
extern struct clock_and_alarm clock_and_alarm;

/**
 * struct temp_and_press - used to work with bmp280 sensor to read temperature and pressure.
 * @temp: temperature value in C
 * @press: pressure value in hPa
 */

struct temp_and_press {
	int32_t temp;
	uint32_t press;
};
extern struct temp_and_press temp_and_press;

/**
 * struct game - used to work with game and pedometer variables, depends on mpu6050  .
 * @x: coordinate of snake by x-axis
 * @y: coordinate of snake by y-axis
 * @dir_x: current snake moving bias by x-axis
 * @dir_y: current snake moving bias by y-axis
 * @tail_x: snake's tail coordinate x array
 * @tail_y: snake's tail coordinate y array
 * @len: snake's tail current lenght, also used for game score
 * @fruit_x: coordinate of fruit by x-axis
 * @fruit_y: coordinate of fruit by y-axis
 * @is_fruit: true is fruit spawned
 * @game_over: true when game is over.
 * @steps_count: pedometer count steps.
 */

struct game {
	int16_t x;
	int16_t y;
	int8_t dir_x;
	int8_t dir_y;
	uint16_t tail_x[SNAKE_TAIL_MAX];
	uint16_t tail_y[SNAKE_TAIL_MAX];
	size_t len;
	uint16_t fruit_x;
	uint16_t fruit_y;
	uint16_t is_fruit;
	uint8_t game_over;
	uint32_t steps_count;
};
extern struct game game;

/**
 * struct options - used to keep options byte from/in register '0xb' rtc dc3231.
 * @is_ampm: if true time format will be in AM/PM, otherwise will show 24 hours
 * @is_alarm_enabled: if true alarm is set to be triggered
 * @is_temp_celsius: if true temperature will be shown in Celsius, othwerwise in Farengheit
 */
struct options {
	uint8_t is_ampm:1;
	uint8_t is_alarm_enabled:1;
	uint8_t is_temp_celsius:1;
};
extern struct options options;

/**
 * struct fs_buffer - our device sysfs file read/write buffer.
 * @buf: if true time format will be in AM/PM, otherwise will show 24 hours
 * @buf_len: if true alarm is set to be triggered
 */
struct fs_buffer {
	char *buf;
	uint32_t buf_len; // use fixed 4 bytes 
};
extern struct fs_buffer fs_buffer;

/**
 * Init device sub modules
 */
extern int  st7735fb_init(void);
extern int  gpio_button_init(void);
extern void init_controls(void);
extern int  sensors_init(void);

/**
 * Unload device sub modules
 */
extern void st7735fb_unload(void);
extern void gpio_button_unload(void);
extern void sensors_unload(void);
extern void controls_unload(void);


/**
 * External functions for display driver
 */
extern void st7735fb_blank_vmem(void); // erase display vmem
extern void st7735fb_draw_string(char *word, uint16_t x, uint16_t y, struct Bitmap *font, uint8_t x_offset, uint16_t color); // draw string to display vmem
extern void draw_icon(uint8_t icon, uint16_t x, uint16_t y, uint16_t color); // draw icon to display vmem
extern void st7735fb_draw_rectangle(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color, uint8_t filled); // draw rectangle or line to display vmem
extern void st7735fb_update_display(void); // update display view
extern int st7735fb_send_buff_display(void);
extern int st7735fb_get_buff_display(void);

/**
 * External functions for ds3231 rtc sensor
 */
extern void ds3231_writeRtcTimeAndAlarm(void);
extern void ds3231_writeOptions(void);

/**
 * External control functions
 */
extern void show_clock_view(void);
extern void show_timer_view(void);
extern void show_alarm_view(void);
extern void show_temp_and_press_view(void);
extern void show_pedometer_view(void);
extern void show_game_view(void);
extern void show_options_view(void);

#endif
