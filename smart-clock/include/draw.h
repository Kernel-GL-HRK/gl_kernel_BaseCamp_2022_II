/* SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef __DRAW_H_INCLUDED
#define __DRAW_H_INCLUDED

/* Including fonts and icons used for display views */
#include "font8.h"
#include "font16.h"
#include "font24.h"
#include "font32.h"
#include "font48.h"
#include "icons.h"

/* 16bit colors for display */
#define  WHITE_COLOR	0xffff	//white
#define  RED_COLOR	0xf800	//red
#define  GREEN_COLOR	0x07e0	//green
#define  BLUE_COLOR	0x00ff	//blue
#define  YELLOW_COLOR	0xffe0	//yellow

/* Colors for imaging */
#define DISP_CLOCK_COLOR		BLUE_COLOR
#define DISP_ALARM_COLOR		BLUE_COLOR
#define DISP_TIMER_COLOR		BLUE_COLOR
#define DISP_TEMP_COLOR		BLUE_COLOR
#define DISP_PRESS_COLOR		BLUE_COLOR
#define DISP_PEDOMETER_COLOR		BLUE_COLOR
#define DISP_OPTIONS_COLOR		BLUE_COLOR
#define DISP_NOTIF_COLOR		GREEN_COLOR

/* Define fonts and sizes used */
static struct Bitmap font[] = {
	{ font8_table, 8, 8},
	{ font16_table, 11, 16},
	{ font24_table, 17, 24 },
	{ font32_table, 27, 34 },
	{ font48_table, 24, 48 },
};

/**
 * enum fonts  used
 * font8.h
 * font16.h
 * font24.h
 * font32.h
 * font48.h
 */
enum fonts {
	FONT8 = 0,
	FONT16,
	FONT24,
	FONT32,
	FONT48,
};
/* Define icons and sizes used */

static struct Bitmap icons = { icons_table, 16, 16 };

/**
 * enum icons -in
 * icons.h
 */
enum icons {
	ALARM_NOTIF = 0,
	TIMER_NOTIF,
	OPTIONS_UNCHECKED,
	OPTIONS_CHECKED,
	GAME_SNAKE_HEAD_UP,
	GAME_SNAKE_HEAD_LEFT,
	GAME_SNAKE_HEAD_DOWN,
	GAME_SNAKE_HEAD_RIGHT,
	GAME_FRUIT,
	GAME_SNAKE_TAIL,
};
#endif
