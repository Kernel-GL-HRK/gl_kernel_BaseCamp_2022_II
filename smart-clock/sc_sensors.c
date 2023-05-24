// SPDX-License-Identifier: GPL-2.0-only
/*
 * Module library: sc_sensors
 * Description: sensors driver for bmp280, mpu6050 and rtc ds3231. Designed for the module.
 * Module: smart-clock
 *
 * Copyright (C) 2023 Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ktime.h>
#include <linux/interrupt.h>

#include "include/bmp280.h"
#include "include/ds3231.h"
#include "include/mpu6050.h"
#include "include/project1.h"


#define BMP280_DEVICE_NAME	"bmp280"
#define DC3231_DEVICE_NAME	"ds3231"
#define MPU6050_DEVICE_NAME	"mpu6050"

/* sensors update delays */
#define BMP280_THREAD_SLEEP	(10000) //ms check temp and pressure values once in 10 sec
#define MPU6050_THREAD_SLEEP_GAME	(30) //ms accel update during the game
#define MPU6050_THREAD_SLEEP_PEDOMETER  (100) //ms accel update when not in game

#define I2C_DATA_BUFFER_MAX	(10)	//bytes

#define ABS(x) (x < 0 ? -x : x)	//Absolute number



static struct mutex i2c_read;
static struct task_struct *bmp280_read_thread, *mpu6050_read_thread;



/**
 * i2c_write_data()
 *
 * returns 0 on successreturn the write byte or word value on success
 */
static int i2c_write_data(struct i2c_client *client, uint8_t command, uint8_t *data, size_t size)
{
	return i2c_smbus_write_i2c_block_data(client, command, size, data);
}

/**
 * i2c_read_data()
 *
 * returns 0 on successreturn the read byte or word value on success
 */
static int i2c_read_data(struct i2c_client *client, uint8_t command, uint8_t *data, size_t size)
{
	return i2c_smbus_read_i2c_block_data(client, command, size, data);
}

/**
 * i2c_read_byte()
 *
 * returns 0 on success
 */
static int i2c_read_byte(struct i2c_client *client, uint8_t command)
{
	return i2c_smbus_read_byte_data(client, command);
}

/**
 * i2c_write_byte()
 *
 * returns 0 on success
 */
static int i2c_write_byte(struct i2c_client *client, uint8_t command, uint8_t data)
{
	return i2c_smbus_write_byte_data(client, command, data);
}


/*----------------------------------------------------------------------*/
/* bmp280 part								*/
/*----------------------------------------------------------------------*/


/**
 * bmp280_read_temp_and_press() - read bmp280 sensor values thread
 *
 */
int bmp280_read_temp_and_press(void *pv)
{
	uint8_t data_buf[I2C_DATA_BUFFER_MAX];
	uint8_t pmsb, plsb, pxsb, tmsb, tlsb, txsb;
	uint32_t  adc_t, adc_p, t_fine;
	uint64_t var1, var2, p;


	while (!kthread_should_stop()) {

		mutex_lock(&i2c_read);
		/*  Read registers */
		i2c_read_data(bmp280_i2c, BMP280_REG_PRESSURE_MSB, data_buf, 6);
		mutex_unlock(&i2c_read);

		pmsb = data_buf[0]; // BMP280_REG_PRESSURE_MSB
		plsb = data_buf[1]; // BMP280_REG_PRESSURE_LSB
		pxsb = data_buf[2]; // BMP280_REG_PRESSURE_XLSB
		tmsb = data_buf[3]; // BMP280_REG_TEMP_MSB
		tlsb = data_buf[4]; // BMP280_REG_TEMP_LSB
		txsb = data_buf[5]; // BMP280_REG_TEMP_XLSB

		/* Convert pressure and temperature data to 19-bits */
		adc_t = ((((tmsb << 8) | tlsb)<<8) | txsb)>>4;
		adc_p = ((((pmsb << 8) | plsb)<<8) | pxsb)>>4;

		/* Calculate temperature */
		var1 = ((((adc_t >> 3) - ((int32_t)calib_data.T1 << 1))) *
		((int32_t)calib_data.T2)) >> 11;
		var2 = (((((adc_t >> 4) - ((int32_t)calib_data.T1)) *
		  ((adc_t >> 4) - ((int32_t)calib_data.T1))) >> 12) *
		((int32_t)calib_data.T3)) >> 14;
		t_fine = (uint32_t)var1+(uint32_t)var2;

		temp_and_press.temp = ((t_fine * 5 + 128) >> 8);

		/* Calculate pressure */
		var1 = (int64_t)t_fine - 128000;
		var2 = var1 * var1 * (int64_t)calib_data.P6;
		var2 += (var1 * (int64_t)calib_data.P5) << 17;
		var2 += ((int64_t)calib_data.P4) << 35;
		var1 = ((var1 * var1 * (int64_t)calib_data.P3) >> 8) +
		((var1 * (int64_t)calib_data.P2) << 12);
		var1 = ((((int64_t)1) << 47) + var1) * (int64_t)calib_data.P1 >> 33;

		p = ((((int64_t)1048576 - adc_p) << 31) - var2) * 3125;
		p = div64_s64(p, var1);
		var1 = ((int64_t)calib_data.P9 * (p >> 13) * (p >> 13)) >> 25;
		var2 = ((int64_t)calib_data.P8 * p) >> 19;
		temp_and_press.press = (uint32_t)((p + var1 + var2) >> 8) + ((int64_t)calib_data.P7 << 4);

		msleep(BMP280_THREAD_SLEEP);
	}
	return 0;
}

/**
 * bmp280_probe() - probe and calibrate bmp280 sensor
 *
 */
static int bmp280_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

	pr_err("%s: probing\n", BMP280_DEVICE_NAME);

	/*reporting probe to init */
	if (i2c_read_byte(client, 0x00) < 0) {
		pr_err("%s: probe failed\n", BMP280_DEVICE_NAME);
		return -ENODEV;
		}
	else {
		init_hw.bmp280_i2c_probed = 1;
		pr_err("%s: probed\n", BMP280_DEVICE_NAME);
		}

	 bmp280_i2c = client;

	/*get calibration */
	calib_data.T1 = (i2c_read_byte(bmp280_i2c, BMP280_REG_T1_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_T1_MSB) << 8);
	calib_data.T2 = (i2c_read_byte(bmp280_i2c, BMP280_REG_T2_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_T2_MSB) << 8);
	calib_data.T3 = (i2c_read_byte(bmp280_i2c, BMP280_REG_T3_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_T3_MSB) << 8);
	calib_data.P1 = (i2c_read_byte(bmp280_i2c, BMP280_REG_P1_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_P1_MSB) << 8);
	calib_data.P2 = (i2c_read_byte(bmp280_i2c, BMP280_REG_P2_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_P2_MSB) << 8);
	calib_data.P3 = (i2c_read_byte(bmp280_i2c, BMP280_REG_P3_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_P3_MSB) << 8);
	calib_data.P4 = (i2c_read_byte(bmp280_i2c, BMP280_REG_P4_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_P4_MSB) << 8);
	calib_data.P5 = (i2c_read_byte(bmp280_i2c, BMP280_REG_P5_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_P5_MSB) << 8);
	calib_data.P6 = (i2c_read_byte(bmp280_i2c, BMP280_REG_P6_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_P6_MSB) << 8);
	calib_data.P7 = (i2c_read_byte(bmp280_i2c, BMP280_REG_P7_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_P7_MSB) << 8);
	calib_data.P8 = (i2c_read_byte(bmp280_i2c, BMP280_REG_P8_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_P8_MSB) << 8);
	calib_data.P9 = (i2c_read_byte(bmp280_i2c, BMP280_REG_P9_LSB) | i2c_read_byte(bmp280_i2c, BMP280_REG_P9_MSB) << 8);

	i2c_write_byte(bmp280_i2c, BMP280_REG_CTRL_MEAS, 0x27);
	udelay(100);
	bmp280_read_thread = kthread_run(bmp280_read_temp_and_press, NULL, "bmp280 read temp&pressure thread");
    return 0;
}


/*----------------------------------------------------------------------*/
/* bmp280 part	END							*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* dc3231 part	START							*/
/*----------------------------------------------------------------------*/


/**
 * bmp280_read_temp_and_press() - read bmp280 sensor values thread
 *
 *
 */
static void dc3231_writeRtcTimeAndAlarm_work(struct work_struct *work)
{
	struct timespec64 curr_tm;
	struct tm tm_now;
	uint8_t data_buf[I2C_DATA_BUFFER_MAX];

	/* Store time and alarm values to RTC registers  */
	ktime_get_real_ts64(&curr_tm);
	time64_to_tm(curr_tm.tv_sec, 0, &tm_now);
	data_buf[0] = val2rtc(tm_now.tm_sec);		// DS3231_REG_SEC
	data_buf[1] = val2rtc(tm_now.tm_min);		// DS3231_REG_MIN
	data_buf[2] = val2rtc(tm_now.tm_hour);		// DS3231_REG_HOUR
	data_buf[3] = val2rtc(tm_now.tm_wday);		// DS3231_REG_WDAY
	data_buf[4] = val2rtc(tm_now.tm_mday);		// DS3231_REG_DAY
	data_buf[5] = val2rtc(tm_now.tm_mon);		// DS3231_REG_MON
	data_buf[6] = val2rtc(tm_now.tm_year);	// DS3231_REG_YEAR
	data_buf[7] = 0;					// DS3231_REG_ALARM_SEC
	data_buf[8] = val2rtc((clock_and_alarm.alarm_sec%3600)/60);	// DS3231_REG_ALARM_MIN
	data_buf[9] = val2rtc(clock_and_alarm.alarm_sec/3600);	// DS3231_REG_ALARM_HOUR
	i2c_write_data(ds3231.client, DS3231_REG_SEC, data_buf, 10);

}

/**
 * ds3231_readRtcTimeAndAlarm() - time and alarm values from rtc
 *
 */
static void ds3231_readRtcTimeAndAlarm(void)
{

	uint8_t data_buf[I2C_DATA_BUFFER_MAX];
	/* Read time and alarm values from RTC registers */

	i2c_read_data(ds3231.client, DS3231_REG_SEC, data_buf, 10);
	ds3231.sec = rtc2val(data_buf[0]);			//0=DS3231_REG_SEC
	ds3231.min = rtc2val(data_buf[1]);			//1=DS3231_REG_MIN
	ds3231.hour = rtc2val(data_buf[2]);			//2=DS3231_REG_HOUR
	ds3231.wday = rtc2val(data_buf[3]);			//3=DS3231_REG_WDAY
	ds3231.mday = rtc2val(data_buf[4]);			//4=DS3231_REG_DAY
	ds3231.mon = rtc2val(data_buf[5]);			//5=DS3231_REG_MON
	ds3231.year = rtc2val(data_buf[6]);			//6=DS3231_REG_YEAR
	clock_and_alarm.alarm_sec = (rtc2val(data_buf[9])*3600+rtc2val(data_buf[8])*60);	//8=DS3231_REG_ALARM_MIN & 9=DS3231_REG_ALARM_HOUR
}

/**
 * dc3231_writeOptions_work() - write options workqueue
 *
 * there is no eeprom in this ds3231, so using register 0x0b to store and read options
 */
static void dc3231_writeOptions_work(struct work_struct *work)
{
	/*convert struct options to byte and store to reg*/
	uint8_t options_info;

	memcpy(&options_info, &options, sizeof(options_info));
	i2c_write_byte(ds3231.client, DS3231_REG_OPTIONS, options_info);
}

DECLARE_WORK(dc3231_writeRtcTimeAndAlarm, dc3231_writeRtcTimeAndAlarm_work);
DECLARE_WORK(dc3231_writeOptions, dc3231_writeOptions_work);

/**
 * ds3231_readOptions() - read options from rtc
 *
 * there is no eeprom in this ds3231, so using register 0x0b to store and read options
 */
static void ds3231_readOptions(void)
{
	/*read byte from reg and store as struct*/
	uint8_t options_info;

	options_info = i2c_read_byte(ds3231.client, DS3231_REG_OPTIONS);
	memcpy(&options, &options_info, sizeof(options_info));
}

/**
 * ds3231_writeOptions() - schedule work for write options
 *
 * externally can be called from controls library
 */
void ds3231_writeOptions(void)
{
	schedule_work(&dc3231_writeOptions);
}

/**
 * ds3231_writeOptions() - schedule work for write Time and Alarm
 *
 * externally can be called from controls library
 */
void ds3231_writeRtcTimeAndAlarm(void)
{
	schedule_work(&dc3231_writeRtcTimeAndAlarm);
}


/**
 * ds3231_probe() - probe and sync kernel time with tme from rtc
 *
 */
static int ds3231_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct timespec64 curr_tm_set = {};

	pr_err("%s: probing\n", DC3231_DEVICE_NAME);

	/*reporting probe to init */
	if (i2c_read_byte(client, 0x00) < 0)	{
		pr_err("%s: probe failed\n", DC3231_DEVICE_NAME);
		return -ENODEV;
	}
	init_hw.ds3231_i2c_probed = 1;
	pr_err("%s: probed\n", DC3231_DEVICE_NAME);

	ds3231.client = client;
	ds3231_readRtcTimeAndAlarm();
	curr_tm_set.tv_sec = mktime64(ds3231.year + 1900, ds3231.mon + 1, ds3231.mday,
			    ds3231.hour, ds3231.min, ds3231.sec);
	/* Set time of the day from RTC. If fail go on, because it can be while battery if low */		    
	pr_err("%s: set time of the day from RTC result %d\n", DC3231_DEVICE_NAME, do_settimeofday64(&curr_tm_set));

	ds3231_readOptions();
	return 0;
}


/*----------------------------------------------------------------------*/
/* dc3231 part	END							*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* mpu6050 part	START							*/
/*----------------------------------------------------------------------*/


/**
 * mpu6050_read() - read mpu6050 sensor values thread
 *
 * used to get and calcualate positions for game and pedometer
 */
int mpu6050_read(void *pv)
{
	int8_t total_vector;
	int8_t accel_delta_x;
	int8_t accel_delta_y;


	while (!kthread_should_stop()) {

		/* Read mpu6050 */
		mutex_lock(&i2c_read);
		mpu6050.accel_x = (i2c_read_byte(mpu6050.client, MPU6050_ACCEL_XOUT_H) | i2c_read_byte(mpu6050.client, MPU6050_ACCEL_XOUT_H+1) << 8);
		mpu6050.accel_y = (i2c_read_byte(mpu6050.client, MPU6050_ACCEL_YOUT_H) | i2c_read_byte(mpu6050.client, MPU6050_ACCEL_YOUT_H+1) << 8);
		mpu6050.accel_z = (i2c_read_byte(mpu6050.client, MPU6050_ACCEL_ZOUT_H) | i2c_read_byte(mpu6050.client, MPU6050_ACCEL_YOUT_H+1) << 8);
		mpu6050.gyro_x = (i2c_read_byte(mpu6050.client, MPU6050_GYRO_XOUT_H) | i2c_read_byte(mpu6050.client, MPU6050_GYRO_XOUT_H+1) << 8);
		mpu6050.gyro_y = (i2c_read_byte(mpu6050.client, MPU6050_GYRO_YOUT_H) | i2c_read_byte(mpu6050.client, MPU6050_GYRO_YOUT_H+1) << 8);
		mpu6050.gyro_z = (i2c_read_byte(mpu6050.client, MPU6050_GYRO_ZOUT_H) | i2c_read_byte(mpu6050.client, MPU6050_GYRO_ZOUT_H+1) << 8);
		mutex_unlock(&i2c_read);

		/* Calculate moving values for game by comparing of values with defaults */
		accel_delta_x = mpu6050.accel_x_def-mpu6050.accel_x;
		accel_delta_y = mpu6050.accel_y_def-mpu6050.accel_y;

		/* Calculate x & y bias according to accel data */
		if ((ABS(accel_delta_x) > 4) && (game.dir_x == 0)) {
			game.dir_x = ABS(accel_delta_x)/accel_delta_x;
			game.dir_y = 0;
		}

		if ((ABS(accel_delta_y) > 4) && (game.dir_y == 0)) {
			game.dir_y = ABS(accel_delta_y)/accel_delta_y;
			game.dir_x = 0;
		}
		/* Apply x & y coordinates */
		game.x += game.dir_x;
		game.y -= game.dir_y;

		/* Check if snake is not out of game screen bounds*/
		if (game.x > (WIDTH-10))
			game.x = WIDTH-10;
		if (game.x < 20)
			game.x = 10;
		if (game.y > (HEIGHT-1))
			game.y = HEIGHT-1;
		if (game.y < 20)
			game.y = 20;

		/* Detect steps -pedometer func */
		mpu6050.vector[mpu6050.vector_num] = int_sqrt((mpu6050.accel_x * mpu6050.accel_y) + (mpu6050.accel_y *											mpu6050.accel_y) + (mpu6050.accel_z * mpu6050.accel_z));

		total_vector = (mpu6050.vector[!mpu6050.vector_num]-mpu6050.vector[mpu6050.vector_num]);
		if (total_vector > 8)	{
			game.steps_count++;
			mpu6050.vector[mpu6050.vector_num] = 0;
			}
		mpu6050.vector_num =  !mpu6050.vector_num;

		/* Variate msleep time for sensor reading thread to sync with refresh rate for snake game or pedometer views */
		msleep((my_button.view_mode == GAME) ? MPU6050_THREAD_SLEEP_GAME : MPU6050_THREAD_SLEEP_PEDOMETER);
	}
return 0;
}

/**
 * mpu6050_probe() - probe and calibrate mpu6050
 *
 */
static int mpu6050_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{

	pr_err("%s: probing\n", MPU6050_DEVICE_NAME);
	/*reporting probe to init */
	if (i2c_read_byte(client, 0x00) < 0)	{
		pr_err("%s: probe failed\n", MPU6050_DEVICE_NAME);
		return -ENODEV;
	}
	init_hw.mpu6050_i2c_probed = 1;
	pr_err("%s: probed\n", MPU6050_DEVICE_NAME);

	mpu6050.client = client;

	/* Disable sleep mode */
	i2c_write_byte(mpu6050.client, MPU6050_PWR_MGMT_1, 0x0);

	/* Config gyro 250° per second range */
	i2c_write_byte(mpu6050.client, MPU6050_GYRO_CONFIG, 0x0);

	/* Config accelerometer 2D */
	i2c_write_byte(mpu6050.client, MPU6050_ACCEL_CONFIG, 0x0);

	mpu6050_read_thread = kthread_run(mpu6050_read, NULL, "mpu6050 read accel&gyro thread");

	msleep(100);

	/* Calibrate default x,y,z values from first run */
	mpu6050.accel_x_def = mpu6050.accel_x;
	mpu6050.accel_y_def = mpu6050.accel_y;
	mpu6050.accel_z_def = mpu6050.accel_z;
	mpu6050.gyro_x_def = mpu6050.gyro_x;
	mpu6050.gyro_y_def = mpu6050.gyro_y;
	mpu6050.gyro_z_def = mpu6050.gyro_z;

	return 0;
};



/*----------------------------------------------------------------------*/
/* mpu6050 part	START							*/
/*----------------------------------------------------------------------*/


/* pre-register Device table for bmp280 START*/

static const struct of_device_id bmp280_of_match[] = {
	{ .compatible = "smart-clock, bmp280" },
	{ }
};
MODULE_DEVICE_TABLE(of, bmp280_of_match);


static struct i2c_driver bmp280_driver = {
	.driver = {
	    .name   = BMP280_DEVICE_NAME,
	    .owner  = THIS_MODULE,
	    .of_match_table = bmp280_of_match,
	},
	  .probe          = bmp280_probe,
};

/* pre-register Device table for bmp280 END */


/* pre-register Device table for dc3231 START*/

static const struct of_device_id ds3231_of_match[] = {
	{ .compatible = "smart-clock, ds3231" },
	{ }
};
MODULE_DEVICE_TABLE(of, ds3231_of_match);


static struct i2c_driver ds3231_driver = {
	.driver = {
	    .name   = DC3231_DEVICE_NAME,
	    .owner  = THIS_MODULE,
	    .of_match_table = ds3231_of_match,
	},
	   .probe          = ds3231_probe,
};

/* pre-register Device table for dc3231 END */

/* pre-register Device table for mpu6050 START*/

static const struct of_device_id mpu6050_of_match[] = {
	{ .compatible = "smart-clock, mpu6050" },
	{ }
};
MODULE_DEVICE_TABLE(of, mpu6050_of_match);


static struct i2c_driver mpu6050_driver = {
	.driver = {
	    .name   = MPU6050_DEVICE_NAME,
	    .owner  = THIS_MODULE,
	    .of_match_table = mpu6050_of_match,
	},
	   .probe          = mpu6050_probe,
};

/* pre-register Device table for mpu6050 END */

/**
 *  sensors_init() - init module library from main.
 *
 */
int  sensors_init(void)
{
	mutex_init(&i2c_read);
	i2c_add_driver(&bmp280_driver);
	i2c_add_driver(&ds3231_driver);
	i2c_add_driver(&mpu6050_driver);
	return 0;
}

/**
 *  sensors_unload() - exit module library from main.
 *
 */
void  sensors_unload(void)
{
	kthread_stop(bmp280_read_thread);
	kthread_stop(mpu6050_read_thread);
	i2c_del_driver(&bmp280_driver);
	i2c_del_driver(&ds3231_driver);
	i2c_del_driver(&mpu6050_driver);
	pr_err("sensors unloaded\n");
}

