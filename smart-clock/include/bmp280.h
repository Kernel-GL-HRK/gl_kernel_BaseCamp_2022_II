/* SPDX-License-Identifier: GPL-2.0-only
 *
 * bmp280 sensor driver header
 *
 *
 * Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#ifndef __BMP280_H__
#define __BMP280_H__

#define BMP280_REG_CONFIG	0xF5
#define BMP280_REG_PRESSURE_MSB              0xF7  /*Pressure MSB Register */
#define BMP280_REG_PRESSURE_LSB              0xF8  /*Pressure LSB Register */
#define BMP280_REG_PRESSURE_XLSB             0xF9  /*Pressure XLSB Register */
#define BMP280_REG_TEMP_MSB           0xFA  /*Temperature MSB Reg */
#define BMP280_REG_TEMP_LSB           0xFB  /*Temperature LSB Reg */
#define BMP280_REG_TEMP_XLSB          0xFC  /*Temperature XLSB Reg */



/*
 * constants for setPowerMode()
 */
#define BMP280_SLEEP_MODE 0x00
#define BMP280_FORCED_MODE 0x01
#define BMP280_NORMAL_MODE 0x03

/* BMP180 and BMP280 common registers */
#define BMP280_REG_CTRL_MEAS		0xF4
#define BMP280_REG_RESET		0xE0
#define BMP280_REG_ID			0xD0

/* callibration registers */
#define BMP280_REG_T1_LSB 0x88
#define BMP280_REG_T1_MSB 0x89
#define BMP280_REG_T2_LSB 0x8A
#define BMP280_REG_T2_MSB 0x8B
#define BMP280_REG_T3_LSB 0x8C
#define BMP280_REG_T3_MSB 0x8D
#define BMP280_REG_P1_LSB 0x8E
#define BMP280_REG_P1_MSB 0x8F
#define BMP280_REG_P2_LSB 0x90
#define BMP280_REG_P2_MSB 0x91
#define BMP280_REG_P3_LSB 0x92
#define BMP280_REG_P3_MSB 0x93
#define BMP280_REG_P4_LSB 0x94
#define BMP280_REG_P4_MSB 0x95
#define BMP280_REG_P5_LSB 0x96
#define BMP280_REG_P5_MSB 0x97
#define BMP280_REG_P6_LSB 0x98
#define BMP280_REG_P6_MSB 0x99
#define BMP280_REG_P7_LSB 0x9A
#define BMP280_REG_P7_MSB 0x9B
#define BMP280_REG_P8_LSB 0x9C
#define BMP280_REG_P8_MSB 0x9D
#define BMP280_REG_P9_LSB 0x9E
#define BMP280_REG_P9_MSB 0x9F


struct BMP280CalibrationData {
	uint16_t T1;
	int16_t T2;
	int16_t T3;
	uint16_t P1;
	int16_t P2;
	int16_t P3;
	int16_t P4;
	int16_t P5;
	int16_t P6;
	int16_t P7;
	int16_t P8;
	int16_t P9;
} calib_data;

struct	i2c_client *bmp280_i2c;
#endif
