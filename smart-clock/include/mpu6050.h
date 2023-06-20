/* SPDX-License-Identifier: GPL-2.0-only
 *
 * mpu6050 sensor driver header
 *
 *
 * Dmytro Volkov <splissken2014@gmail.com>
 *
 */

#ifndef __MPU6050_H__
#define __MPU6050_H__

/* MPU6050 registers */

#define MPU6050_PWR_MGMT_1     0x6b
#define MPU6050_SMPLRT_DIV     0x19
#define MPU6050_CONFIG         0x1a
#define MPU6050_GYRO_CONFIG    0x1b
#define MPU6050_ACCEL_CONFIG   0x1c
#define MPU6050_INT_ENABLE     0x38
#define MPU6050_ACCEL_XOUT_H   0x3b
#define MPU6050_ACCEL_YOUT_H   0x3d
#define MPU6050_ACCEL_ZOUT_H   0x3f
#define MPU6050_GYRO_XOUT_H    0x43
#define MPU6050_GYRO_YOUT_H    0x45
#define MPU6050_GYRO_ZOUT_H    0x47


static struct mpu6050 {
	struct i2c_client *client;
	int8_t accel_x;
	int8_t accel_y;
	int8_t accel_z;
	int8_t gyro_x;
	int8_t gyro_y;
	int8_t gyro_z;
	int8_t accel_x_def;
	int8_t accel_y_def;
	int8_t accel_z_def;
	int8_t gyro_x_def;
	int8_t gyro_y_def;
	int8_t gyro_z_def;
	uint8_t vector[2];
	uint8_t vector_num;
} mpu6050;


#endif
