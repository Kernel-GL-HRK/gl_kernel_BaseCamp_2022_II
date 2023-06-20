/* SPDX-License-Identifier: GPL-2.0-only
 *
 * ds3231 rtc driver header
 *
 *
 * Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#ifndef __DS3231_H__
#define __DS3231_H__


#define rtc2val(x)	(((x) & 0x0f) + ((x) >> 4) * 10)
#define val2rtc(x)	((((x) / 10) << 4) + (x) % 10)

/* Time registers */

#define DS3231_REG_SEC		0x00
#define DS3231_REG_MIN		0x01
#define DS3231_REG_HOUR		0x02
#define DS3231_REG_WDAY		0x03
#define DS3231_REG_DAY		0x04
#define DS3231_REG_MON		0x05
#define DS3231_REG_YEAR		0x06

/* Alarm registers */

#define DS3231_REG_ALARM_SEC	0x07
#define DS3231_REG_ALARM_MIN	0x08
#define DS3231_REG_ALARM_HOUR	0x09

/* Our options storage registers, since we dont have EEPROM we are using Alarm2 registers 0x0b & 0x0c  */
/* 0x0b 1st bit=enable alarm; 2nd bit=enable clock am/pm; 3d bit=enables temperature shown in farenheits */

#define DS3231_REG_OPTIONS		0x0b

struct ds3231 {
	struct i2c_client *client;
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t wday;
	uint8_t mday;
	uint8_t mon;
	uint8_t year;
	uint8_t alarm_sec;
	uint8_t alarm_min;
	uint8_t alarm_hour;
} ds3231;

#endif
