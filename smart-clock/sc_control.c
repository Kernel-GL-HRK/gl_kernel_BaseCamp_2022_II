// SPDX-License-Identifier: GPL-2.0-only
/*
 * Module library: sc_control
 * Description: View and main control functions for the module
 * Module: smart-clock
 *
 * Copyright (C) 2023 Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#include <linux/timer.h>  //timer

#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct
#include <linux/mutex.h>
#include <linux/random.h>

#include <linux/ktime.h>
#include "include/project1.h"
#include "include/draw.h"



/* Timers callback delays */
#define DISP_TIME_REFRESH_TIME 200	//ms =5 fps
#define DISP_HR_TIMER_REFRESH_TIME 17	//ms =58 fps
#define DISP_ALARM_REFRESH_TIME 200     //ms 5 fps
#define DISP_TEMP_AND_PRESS_REFRESH_TIME 200 //5 fps
#define DISP_PEDOMETER_REFRESH_TIME 200 //5 fps
#define DISP_GAME_REFRESH_TIME 40 //20 fps
#define DISP_OPTIONS_REFRESH_TIME 200	//5 fps
#define ALARM_TRIGGER_REFRESH_TIME 200

/* Max size for screen text values */
#define MAX_STRING_SIZE	(20)

/* ISLP: - leap year detection
 * GDBM: - get max days per month by year
 * NS_TO_MSEC: -64 bit variables division not supported in 32 bit systems, so using macros to get nsec from msec
 * FAHR: to convert Celsius to Fahrenheit
 */
#define SEC		(60)
#define SECS_PER_HOUR	(60 * 60)
#define SECS_PER_DAY	(SECS_PER_HOUR * 24)
#define ISLP(y) (((y % 400 == 0) && (y % 100 == 0)) || (y % 4 == 0))
#define GDBM(m, y) (((m) == 2) ? (ISLP(y) ? 29 : 28) : (((m) < 8) ? (((m) % 2) ? 31 : 30) : (((m) % 2) ? 30 : 31)))
#define NS_PER_MSEC (1000000L)
#define NS_TO_MSEC(nsec) (div_s64(nsec, NS_PER_MSEC))
#define FAHR(x) (9*x/5+32)


const char *wdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

static struct hrtimer digital_timer_view;
static struct timer_list digital_clock_view, options_view, alarm_view, temp_and_press_view, pedometer_view, game_view, alarm_trigger;

uint8_t blink_bmask;

/**
 * show_timer_view() - HR Timer view mode
 *
 */
void show_timer_view(void)
{
	/* Run the timer immidiately to update to new view */
	hrtimer_start(&digital_timer_view, 0, HRTIMER_MODE_REL);
}

/**
 * show_clock_view() - clock view mode
 *
 */
void show_clock_view(void)
{
	/* Run the timer immidiately to update to new view */
	my_button.edit_mode = 0;
	mod_timer(&digital_clock_view,
			jiffies + msecs_to_jiffies(0));
}

/**
 * show_options_view() - options view mode
 *
 */
void show_options_view(void)
{
	/* Run the timer immidiately to update to new view */
	mod_timer(&options_view,
			jiffies + msecs_to_jiffies(0));
}

/**
 * show_alarm_view() - clock timer view mode
 *
 */
void show_alarm_view(void)
{
	/* Run the timer immidiately to update to new view */
	mod_timer(&alarm_view,
			jiffies + msecs_to_jiffies(0));
}

/**
 * show_temp_and_press_view() - temperature and pressure view mode
 *
 */
void show_temp_and_press_view(void)
{
	/* Run the timer immidiately to update to new view */
	mod_timer(&temp_and_press_view,
			jiffies + msecs_to_jiffies(0));
}

/**
 * show_pedometer_view() - clock timer view
 *
 */
void show_pedometer_view(void)
{
	/* Run the timer immidiately to update to new view */
	mod_timer(&pedometer_view,
			jiffies + msecs_to_jiffies(0));
}

/**
 * show_game_view() - gamer view
 *
 * switches to game mode and runs HR timer
 */
void show_game_view(void)
{
	/* Prepare snake to run */
	game.len = 20;
	game.is_fruit = 0;
	game.x = 80;
	game.y = 80;
	game.game_over = 0;

	/* Clear previuos snake tails */
	memset(game.tail_x, 0, sizeof(game.tail_x));
	memset(game.tail_y, 0, sizeof(game.tail_y));

	/* Run the timer the timer immidiately to update to new view */
	mod_timer(&game_view,
			jiffies + msecs_to_jiffies(0));
}

/**
 * digital_timer_view_callback() - HR timer callback for clock timer
 *
 * Timer state: only on in the TIMER view
 */
static enum hrtimer_restart digital_timer_view_callback(struct hrtimer *timer)
{
	/* when button longpress x1 detected erasing timer */
	if ((my_button.state == 2) && (!our_timer.is_timer_on)) {
		our_timer.nsec_stored = 0;
		our_timer.nsec = 0;
		my_button.state = 0;
				}
	/* when button press detected staring time for pausing depending on current state */
	if ((my_button.state == 1) && (our_timer.is_timer_on == 0))  {
		our_timer.is_timer_on =  !our_timer.is_timer_on;
		my_button.state =  !my_button.state;
		our_timer.nsec_old_ktime = ktime_get_real_ns();
		if (our_timer.nsec)
			our_timer.nsec_stored = our_timer.nsec;
	/* stop timer whan button is short pressed */
	} else if ((my_button.state == 1) && (our_timer.is_timer_on == 1))  {
		our_timer.is_timer_on =  !our_timer.is_timer_on;
		my_button.state =  !my_button.state;
				}
	/* timer calculation while timer is running */
	if (our_timer.is_timer_on)
		our_timer.nsec = ktime_get_real_ns()-our_timer.nsec_old_ktime+our_timer.nsec_stored;

	/* if we are in TIMER view update the screen, otherwise stop restarting the timer */
	if (my_button.view_mode == TIMER) {
		static char text[MAX_STRING_SIZE];

		/* clear screen vmem */
		st7735fb_blank_vmem();

		/* Draw timer title to vmem */
		st7735fb_draw_string("Timer:", 0, 20, &font[FONT16], 10, DISP_TIMER_COLOR);

		/* Draw timer to vmem */
		sprintf(text, "%02d:%02d:%03d", (uint32_t)NS_TO_MSEC(our_timer.nsec)/1000/60,
			(uint32_t)NS_TO_MSEC(our_timer.nsec)/1000%60, (uint32_t)NS_TO_MSEC(our_timer.nsec)%1000);
		st7735fb_draw_string(text, 10, 50, &font[FONT32], 14, DISP_TIMER_COLOR);

		/* Update display view */
		st7735fb_update_display();

		hrtimer_forward_now(timer, ms_to_ktime(DISP_HR_TIMER_REFRESH_TIME));
		return HRTIMER_RESTART;
	} else {
		return HRTIMER_NORESTART;
		}
}

/**
 * alarm_trigger_callback() - callback to for alarm trigger
 *
 * Timer state: always on
 */
static void alarm_trigger_callback(struct timer_list *t)
{
	struct timespec64 curr_tm;
	struct tm tm_now;
	uint64_t time_sec;
	uint16_t timer_delay = 0;

	/* Alarm trigger check */
	ktime_get_real_ts64(&curr_tm);
	time64_to_tm(curr_tm.tv_sec, 0, &tm_now);

	time_sec = tm_now.tm_hour*SECS_PER_HOUR + tm_now.tm_min*SEC;


	if ((tm_now.tm_hour == clock_and_alarm.alarm_sec/3600) &&
		(tm_now.tm_min == (clock_and_alarm.alarm_sec%3600)/60) && (!clock_and_alarm.is_alarm))
		clock_and_alarm.is_alarm = 1;

	/* Postprone next trigeer to 65 sec when alarm stopped to avoid notifications loop*/
	if ((clock_and_alarm.is_alarm) && (my_button.state)) {
		clock_and_alarm.is_alarm = 0;
		timer_delay = 0xFFFF;
	}

	mod_timer(&alarm_trigger,
			jiffies + msecs_to_jiffies(ALARM_TRIGGER_REFRESH_TIME+timer_delay));
}

/**
 * digital_clock_view_callback() - timer runs Clock view and edit
 *
 * Timer state: always on
 */
static void digital_clock_view_callback(struct timer_list *t)
{

	struct timespec64 curr_tm;
	struct tm tm_now, tm_stored;

	/* kernel real time is used in clock because is only support to set clock from user values */
	ktime_get_real_ts64(&curr_tm);

	/* when button longpress x1 detected turning on setup mode */
	if (my_button.state == 2 && !my_button.is_longpress && !my_button.edit_mode) {
		my_button.edit_mode = 1;
		my_button.state = 0;

	}

	/* when button longpress x2 detected switching to next setup mode */
	if (my_button.state == 3 && my_button.edit_mode) {
		my_button.edit_mode++;
		my_button.state = 0;
		blink_bmask = 0xFF; //stop blinking
		curr_tm.tv_sec = clock_and_alarm.clock_sec;
		do_settimeofday64(&curr_tm);
		ds3231_writeRtcTimeAndAlarm();

		if (my_button.edit_mode == 6)
			my_button.edit_mode = 0;
	}

	if (!my_button.edit_mode) {
		clock_and_alarm.clock_sec = curr_tm.tv_sec;
		clock_and_alarm.clock_stored_sec = clock_and_alarm.clock_sec;
	}

	time64_to_tm(clock_and_alarm.clock_stored_sec, 0, &tm_stored);

	/* when button press detected switching values depending on current setup mode */
	if (my_button.state == 1) {
		switch (my_button.edit_mode) {
		/*adjust min */
		case 1:
			clock_and_alarm.clock_sec += SEC;
			time64_to_tm(clock_and_alarm.clock_sec, 0, &tm_now);
			if (tm_now.tm_hour != tm_stored.tm_hour)
				clock_and_alarm.clock_sec -= SECS_PER_HOUR;
			break;
		/*adjust hours */
		case 2:
			clock_and_alarm.clock_sec += SECS_PER_HOUR;
			time64_to_tm(clock_and_alarm.clock_sec, 0, &tm_now);
			if (tm_now.tm_mday != tm_stored.tm_mday)
				clock_and_alarm.clock_sec -= SECS_PER_DAY;
			break;
		/*adjust days */
		case 3:
			clock_and_alarm.clock_sec += SECS_PER_DAY;
			time64_to_tm(clock_and_alarm.clock_sec, 0, &tm_now);
			if (tm_now.tm_mon != tm_stored.tm_mon)
				clock_and_alarm.clock_sec -= GDBM(tm_stored.tm_mon+1, tm_stored.tm_year)*SECS_PER_DAY;
			break;
		/*adjust month */
		case 4:
			time64_to_tm(clock_and_alarm.clock_sec, 0, &tm_now);
			clock_and_alarm.clock_sec += GDBM(tm_now.tm_mon+1, tm_now.tm_year)*SECS_PER_DAY;
			time64_to_tm(clock_and_alarm.clock_sec, 0, &tm_now);
			if (tm_now.tm_year != tm_stored.tm_year)
				clock_and_alarm.clock_sec -= (ISLP(tm_stored.tm_year) ? 366*SECS_PER_DAY : 365*SECS_PER_DAY);
			break;
		/*adjust year (max 2050) */
		case 5:
			time64_to_tm(clock_and_alarm.clock_sec, 0, &tm_now);
			clock_and_alarm.clock_sec = mktime64(tm_now.tm_year+1+1900, tm_now.tm_mon + 1, tm_now.tm_mday,
			    tm_stored.tm_hour, tm_stored.tm_min, tm_stored.tm_sec);
			if ((tm_now.tm_year+1) % 100 > 50) // rewind year by 50 when reached 2050
				clock_and_alarm.clock_sec -= (12*366+38*365)*SECS_PER_DAY; //12 leap years and 38 non leap years in 50 years
			break;
			};

		my_button.state = 0;
		}

	time64_to_tm(clock_and_alarm.clock_sec, 0, &tm_now);

	/* Only update diplay view while is in clock view  mode */
	if (my_button.view_mode == CLOCK) {

		static char text[MAX_STRING_SIZE];

		/* clear screen vmem */
		st7735fb_blank_vmem();

		switch (my_button.edit_mode) {
		case 1:
			blink_bmask ^= 1 << 1;  // min blinking
			break;
		case 2:
			blink_bmask ^= 1 << 2;  // hour blinking
			break;
		case 3:
			blink_bmask ^= 1 << 3;  // day blinking
			break;
		case 4:
			blink_bmask ^= 1 << 4;  // month blinking
			break;
		case 5:
			blink_bmask ^= 1 << 5;  // year blinking
			break;
		default:
			blink_bmask = 0xFF;
			}


		/* draw tm_mday */
		sprintf(text, "%s", wdays[tm_now.tm_wday]);
		st7735fb_draw_string(text, 0, 15, &font[FONT24], 12, DISP_CLOCK_COLOR);
		/* draw m_day */
		sprintf(text, "%02d", tm_now.tm_mday);
		st7735fb_draw_string(text, 50, 15, &font[FONT24], 12, ((blink_bmask >> 3) & 1) ? DISP_CLOCK_COLOR : 0);
		/* draw tm_mon */
		sprintf(text, "%02d", tm_now.tm_mon+1);
		st7735fb_draw_string(text, 85, 15, &font[FONT24], 12, ((blink_bmask >> 4) & 1) ? DISP_CLOCK_COLOR : 0);
		/* draw tm_year */
		sprintf(text, "%02d", (uint16_t) tm_now.tm_year % 100);
		st7735fb_draw_string(text, 120, 15, &font[FONT24], 12, ((blink_bmask >> 5) & 1) ? DISP_CLOCK_COLOR : 0);
		/* draw date dividers  */
		sprintf(text, "/  /");
		st7735fb_draw_string(text, 75, 15, &font[FONT24], 11, DISP_CLOCK_COLOR);

		if (!options.is_ampm) {
			/* draw tm_hour */
			sprintf(text, "%02d", tm_now.tm_hour);
			st7735fb_draw_string(text, 0, 40, &font[FONT48], 22, ((blink_bmask >> 2) & 1) ? DISP_CLOCK_COLOR : 0);
			/* draw tm_min */
			sprintf(text, "%02d", tm_now.tm_min);
			st7735fb_draw_string(text, 55, 40, &font[FONT48], 22, ((blink_bmask >> 1) & 1) ? DISP_CLOCK_COLOR : 0);
			/* draw tm_sec */
			sprintf(text, "%02d", tm_now.tm_sec % 100);
			st7735fb_draw_string(text, 110, 40, &font[FONT48], 22, DISP_CLOCK_COLOR);
			/* draw time separators  */
			sprintf(text, ":  :");
			st7735fb_draw_string(text, 38, 38, &font[FONT48], 18, DISP_CLOCK_COLOR);
		} else {
			sprintf(text, "%2d", (tm_now.tm_hour != 12) ? (tm_now.tm_hour%12) : 12);
			st7735fb_draw_string(text, 0, 50, &font[FONT32], 14, ((blink_bmask >> 2) & 1) ? DISP_CLOCK_COLOR : 0);
			/* draw tm_min */
			sprintf(text, "%02d", tm_now.tm_min);
			st7735fb_draw_string(text, 37, 50, &font[FONT32], 14, ((blink_bmask >> 1) & 1) ? DISP_CLOCK_COLOR : 0);
			/* draw tm_sec */
			sprintf(text, "%02d", tm_now.tm_sec % 100);
			st7735fb_draw_string(text, 75, 50, &font[FONT32], 14, DISP_CLOCK_COLOR);
			/* draw time separators  */
			sprintf(text, ":  :");
			st7735fb_draw_string(text, 25, 50, &font[FONT32], 13, DISP_CLOCK_COLOR);
			/* draw am/pm  */
			strcpy(text, (tm_now.tm_hour > 12 ? "PM":"AM"));
			st7735fb_draw_string(text, 110, 60, &font[FONT24], 12, DISP_CLOCK_COLOR);
		}
		/* Update display view */
		st7735fb_update_display();

		/* Enable next timer call */
		mod_timer(&digital_clock_view,
			jiffies + msecs_to_jiffies(DISP_TIME_REFRESH_TIME));
	}

}

/**
 * dalarm_view_callback() - timer runs Alarm and Alarm edit view
 *
 * Timer state: always on
 */
static void alarm_view_callback(struct timer_list *t)
{
	uint8_t alarm_hour, alarm_min;

	/* when button longpress x1 detected turning on setup mode */
	if (my_button.state == 2 && !my_button.is_longpress && !my_button.edit_mode) {
		my_button.edit_mode = 1;
		my_button.state = 0;
	}

	/* when button longpress x2 detected switching to next setup mode */
	if (my_button.state == 3 && my_button.edit_mode) {
		my_button.edit_mode++;
		my_button.state = 0;
		blink_bmask = 0xFF; //stop blinking
		ds3231_writeRtcTimeAndAlarm();
		if (my_button.edit_mode == 3)
			my_button.edit_mode = 0;
	}
	/* when button press detected switching values depending on current setup mode */
	if (my_button.state == 1) {
		switch (my_button.edit_mode) {
		/*adjust min */
		case 1:
			clock_and_alarm.alarm_sec += SEC;
			if (clock_and_alarm.alarm_stored_sec/SECS_PER_HOUR != clock_and_alarm.alarm_sec/SECS_PER_HOUR)
				clock_and_alarm.alarm_sec -= SECS_PER_HOUR;
			break;
		/*adjust hours */
		case 2:
			clock_and_alarm.alarm_sec += SECS_PER_HOUR;
			if (clock_and_alarm.alarm_stored_sec/SECS_PER_DAY != clock_and_alarm.alarm_sec/SECS_PER_DAY)
				clock_and_alarm.alarm_sec -= SECS_PER_DAY;
			break;
						}
		my_button.state = 0;
		}
	if (!my_button.edit_mode)
		clock_and_alarm.alarm_stored_sec = clock_and_alarm.alarm_sec;

	alarm_hour = clock_and_alarm.alarm_sec/3600;
	alarm_min = (clock_and_alarm.alarm_sec%3600)/60;

	/* Only update diplay view while is in Alarm view mode */
	if (my_button.view_mode == ALARM) {

		static char text[MAX_STRING_SIZE];


		switch (my_button.edit_mode) {
		case 1:
			blink_bmask ^= 1 << 1;  // min
			break;
		case 2:
			blink_bmask ^= 1 << 2;  // hour
			break;
		default:
			blink_bmask = 0xFF;
			}

		/* clear screen */
		st7735fb_blank_vmem();

		/* draw alarm title to vmem */
		st7735fb_draw_string("Alarm:", 0, 20, &font[FONT24], 14, DISP_ALARM_COLOR);

		/* draw alarm hour depending from am/pm option to vmem */
		if (options.is_ampm) {
			strcpy(text, (alarm_hour > 12 ? "PM":"AM"));
			st7735fb_draw_string(text, 110, 55, &font[FONT24], 22, DISP_ALARM_COLOR);
			/* draw tm_hour to vmem*/
			sprintf(text, "%2d", (alarm_hour != 12) ? (alarm_hour%12) : 12);
		} else
			sprintf(text, "%02d", alarm_hour);

		st7735fb_draw_string(text, 0, 40, &font[FONT48], 22, ((blink_bmask >> 2) & 1) ? DISP_ALARM_COLOR : 0);
		/* draw alarm min to vmem*/
		sprintf(text, "%02d", alarm_min);
		st7735fb_draw_string(text, 55, 40, &font[FONT48], 22, ((blink_bmask >> 1) & 1) ? DISP_ALARM_COLOR : 0);

		/* draw time separators to vmem  */
		sprintf(text, ":");
		st7735fb_draw_string(text, 38, 38, &font[FONT48], 18, DISP_ALARM_COLOR);

		/* Update display view */
		st7735fb_update_display();
		/* Enable next timer call */
		mod_timer(&alarm_view,
			jiffies + msecs_to_jiffies(DISP_TIME_REFRESH_TIME));
	}
}


/**
 * options_view_callback() - timer runs at options view mode
 *
 * Timer state: always on
 */
static void options_view_callback(struct timer_list *t)
{
	/* when button longpress x1 detected turning on setup mode */
	if (my_button.state == 2 && !my_button.is_longpress && !my_button.edit_mode) {
		my_button.edit_mode = 1;
		my_button.state = 0;
	}

	/* when button longpress x2 detected switching to next setup mode */
	if (my_button.state == 3 && my_button.edit_mode) {
		my_button.edit_mode++;
		my_button.state = 0;
		blink_bmask = 0xFF; //stop blinking
		ds3231_writeOptions();
		if (my_button.edit_mode == 4)
			my_button.edit_mode = 0;
	}
	/* when button press detected switching values depending on current setup mode */
	if (my_button.state == 1) {
		switch (my_button.edit_mode) {
		case 1:
			options.is_ampm = !options.is_ampm;
			break;
		case 2:
			options.is_alarm_enabled = !options.is_alarm_enabled;
			break;
		case 3:
			options.is_temp_celsius = !options.is_temp_celsius;
			break;
		}
		my_button.state = 0;
		}
	/* Only update diplay view while is in Options view mode */
	if (my_button.view_mode == OPTIONS) {

		/* Set option blinking if in edit mode */
		switch (my_button.edit_mode) {
		case 1:
			blink_bmask ^= 1 << 1;  // option clock 24hrs blinking
			break;
		case 2:
			blink_bmask ^= 1 << 2;  // option alarm blinking
			break;
		case 3:
			blink_bmask ^= 1 << 3;  // option celsius blinking
			break;
		default:
			blink_bmask = 0xFF;
				}

		/* clear screen */
		st7735fb_blank_vmem();
		/* Draw Options Title to vmem */
		st7735fb_draw_string("Options:", 0, 20, &font[FONT24], 14, DISP_OPTIONS_COLOR);
		/* Draw option is clock 24 hours */
		st7735fb_draw_string("clock 24 hours", 25, 55, &font[FONT16], 9, DISP_OPTIONS_COLOR);
		/* Draw option is alarm enabled */
		st7735fb_draw_string("enable alarm", 25, 75, &font[FONT16], 9, DISP_OPTIONS_COLOR);
		/* Draw option is temp in celsius */
		st7735fb_draw_string("temp in celsius", 25, 95, &font[FONT16], 9, DISP_OPTIONS_COLOR);
		/* draw icon is option enabled */
		draw_icon((options.is_ampm ? OPTIONS_UNCHECKED : OPTIONS_CHECKED), 5, 55, ((blink_bmask >> 1) & 1) ? DISP_OPTIONS_COLOR : 0);
		/* draw icon is option enabled */
		draw_icon((options.is_alarm_enabled ? OPTIONS_CHECKED : OPTIONS_UNCHECKED), 5, 75, ((blink_bmask >> 2) & 1) ? DISP_OPTIONS_COLOR : 0);
		/* draw icon is alarm enabled */
		draw_icon((options.is_temp_celsius ? OPTIONS_CHECKED : OPTIONS_UNCHECKED), 5, 95, ((blink_bmask >> 3) & 1) ? DISP_OPTIONS_COLOR : 0);
		/* Update display view */
		st7735fb_update_display();
		/* Enable next timer call */
		mod_timer(&options_view,
			jiffies + msecs_to_jiffies(DISP_OPTIONS_REFRESH_TIME));
	}
}

/**
 * temp_and_press_view_callback() - timer runs at temperature and pressure view mode
 *
 * Timer state: always on
 */
static void temp_and_press_view_callback(struct timer_list *t)
{
	/* Only update diplay view while is in temp and press view mode */
	if (my_button.view_mode == TEMP_AND_PRESS) {
		static char text[MAX_STRING_SIZE];

		/* clear screen */
		st7735fb_blank_vmem();
		/* Draw Temperature Title to vmem */
		st7735fb_draw_string("Temperature:", 0, 20, &font[FONT16], 10, DISP_TEMP_COLOR);
		/* Display temperature in C or in Farenheight  */
		if (options.is_temp_celsius)
			sprintf(text, "%d,%d C", temp_and_press.temp/100, temp_and_press.temp%100);
		else
			sprintf(text, "%d F", FAHR(temp_and_press.temp/100));
		st7735fb_draw_string(text, 20, 35, &font[FONT24], 12, DISP_TEMP_COLOR);
		/* Draw Pressure Title to vmem */
		st7735fb_draw_string("Pressure:", 0, 60, &font[FONT16], 10, DISP_PRESS_COLOR);
		sprintf(text, "%d,%d hPa", temp_and_press.press/256/100, temp_and_press.press/256%100);
		/* Display pressure  */
		st7735fb_draw_string(text, 20, 75, &font[FONT24], 12, DISP_PRESS_COLOR);
		/* Update display view */
		st7735fb_update_display();
		/* Enable next timer call */
		mod_timer(&temp_and_press_view,
			jiffies + msecs_to_jiffies(DISP_TEMP_AND_PRESS_REFRESH_TIME));
	}
}

/**
 * pedometer_view_callback() - timer runs at Pedometer view mode
 *
 * Timer state: always on
 */
static void pedometer_view_callback(struct timer_list *t)
{

	/* Reset pedometer when button is pressed */
	if (my_button.state == 1) {
		game.steps_count = 0;
		my_button.state = 0;

	}

	/* Only update diplay view while is in Pedometer view mode */
	if (my_button.view_mode == PEDOMETER) {
		static char text[MAX_STRING_SIZE];

		/* clear screen */
		st7735fb_blank_vmem();
		/* Draw Pedometer Title to vmem */
		st7735fb_draw_string("Pedometer:", 0, 20, &font[FONT16], 10, DISP_PEDOMETER_COLOR);
		sprintf(text, "%d", game.steps_count);
		/* Display Pedometer value  */
		st7735fb_draw_string(text, 20, 35, &font[FONT24], 12, DISP_PEDOMETER_COLOR);
		/* Update display view */
		st7735fb_update_display();
		/* Enable next timer call */
		mod_timer(&pedometer_view,
			jiffies + msecs_to_jiffies(DISP_PEDOMETER_REFRESH_TIME));
	}
}

/**
 * game_view_callback() - timer runs at Game view mode
 *
 * Timer state: always on
 */
static void game_view_callback(struct timer_list *t)
{
/* Only update diplay view while is in game view mode */
	if (my_button.view_mode == GAME) {
		if (!game.game_over) {
			static char text[MAX_STRING_SIZE];
			int i;

			/* clear screen */
			st7735fb_blank_vmem();

			/* Spawn fruit if it is not */
			if (!game.is_fruit) {
				get_random_bytes(&game.fruit_x, sizeof(game.fruit_x));
				get_random_bytes(&game.fruit_y, sizeof(game.fruit_y));
				game.fruit_x = game.fruit_x%(WIDTH-15)+10;
				game.fruit_y = game.fruit_x%(HEIGHT-15)+10;
					/* Check fruit spawn point if its not in view merges respawn again */
					if ((game.fruit_x > 20) && (game.fruit_y > 20))
						game.is_fruit = 1;

			} else
			/* Draw fruit */
				draw_icon(GAME_FRUIT, game.fruit_x-8, game.fruit_y-8, RED_COLOR);
			/* Draw snake tail */
			for (i = 0; i < game.len; i++)
				draw_icon(GAME_SNAKE_TAIL, game.tail_x[i]-8, game.tail_y[i]-8, YELLOW_COLOR);

			/* Check if snake hit her tail. if yes reporting about Game Over */
			for (i = 5; i < game.len; i++)
				if ((game.tail_x[i] < game.x+3) &&
						(game.tail_x[i] > game.x-3) &&
						(game.tail_y[i] > game.y-3) &&
						(game.tail_y[i] < game.y+3)) {
					/* Draw Game Over message */
					st7735fb_draw_string("GAME OVER", 30, 50, &font[FONT8], 10, RED_COLOR);
					game.game_over = 1;
					my_button.state = 0;
				}
			/* Remove top value from snake tail stack and replace the first by new one  */
			for (i = game.len; i > 0; i--) {
				game.tail_x[i] = game.tail_x[i-1];
				game.tail_y[i] = game.tail_y[i-1];
				}
			game.tail_x[0] = (uint16_t)game.x;
			game.tail_y[0] = (uint16_t)game.y;

			/* Check if player his the fruit and add +20 to tail len and to score  */
			if (((game.x) < (game.fruit_x+10)) &&
						(game.x > (game.fruit_x-10)) &&
						(game.y > (game.fruit_y-10)) &&
						(game.y < (game.fruit_y+10)) && game.is_fruit)	{
				game.is_fruit = 0;
				game.len += 20;
			}

			/* Depending of moving direction draw appropriate snakes head  */
			if (game.dir_x > 0)
				draw_icon(GAME_SNAKE_HEAD_RIGHT, game.x-8, game.y-8, GREEN_COLOR);
			if (game.dir_x < 0)
				draw_icon(GAME_SNAKE_HEAD_LEFT, game.x-8, game.y-8, GREEN_COLOR);
			if (game.dir_y > 0)
				draw_icon(GAME_SNAKE_HEAD_UP, game.x-8, game.y-8, GREEN_COLOR);
			if (game.dir_y < 0)
				draw_icon(GAME_SNAKE_HEAD_DOWN, game.x-8, game.y-8, GREEN_COLOR);

			/* Draw game frames and score */
			st7735fb_draw_rectangle(0, 0, WIDTH-1, HEIGHT-1, WHITE_COLOR, 0);
			st7735fb_draw_rectangle(0, 0, WIDTH-1, 15, WHITE_COLOR, 0);
			sprintf(text, "%06d", game.len-20);
			st7735fb_draw_string(text, 5, 5, &font[FONT8], 8, WHITE_COLOR);
			/* Update display view */
			st7735fb_update_display();
		} else if (my_button.state == 1)
			show_game_view();
		/* Enable next timer call */
		mod_timer(&game_view,
			jiffies + msecs_to_jiffies(DISP_GAME_REFRESH_TIME));
	}

}

/**
 *  init_controls() - init module library from main.
 *
 */
void init_controls(void)
{

	/*setup view clock mode refresh timer */
	timer_setup(&digital_clock_view, digital_clock_view_callback, 0);

	/*setup view HR timer for clock timer refresh mode */
	hrtimer_init(&digital_timer_view, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	digital_timer_view.function = &digital_timer_view_callback;

	/*setup alarm view mode refresh timer */
	timer_setup(&alarm_view, alarm_view_callback, 0);

	/*setup temperature and pressure view mode refresh timer */
	timer_setup(&temp_and_press_view, temp_and_press_view_callback, 0);

	/*setup temperature and pressure view mode refresh timer */
	timer_setup(&pedometer_view, pedometer_view_callback, 0);

	/*setup game view mode refresh timer */
	timer_setup(&game_view, game_view_callback, 0);

	/*setup view options mode refresh timer */
	timer_setup(&options_view, options_view_callback, 0);

	/*setup and run alarm clock trigger timer */
	timer_setup(&alarm_trigger, alarm_trigger_callback, 0);
	mod_timer(&alarm_trigger,
			jiffies + msecs_to_jiffies(ALARM_TRIGGER_REFRESH_TIME));

}

/**
 *  controls_unload() - exit module library from main.
 *
 */
void  controls_unload(void)
{
	del_timer_sync(&alarm_trigger);
	del_timer_sync(&game_view);
	del_timer_sync(&pedometer_view);
	del_timer_sync(&temp_and_press_view);
	del_timer_sync(&digital_clock_view);
	hrtimer_cancel(&digital_timer_view);
	del_timer_sync(&options_view);
	del_timer_sync(&alarm_view);
	msleep(100);
	pr_err("controls unloaded\n");
}


