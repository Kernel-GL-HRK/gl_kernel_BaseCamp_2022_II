// SPDX-License-Identifier: GPL-2.0
/*
 * This is a simple mpu6050 gyroscope kernel device driver.
 *
 * For detail's see next link's:
 * https://www.cs.swarthmore.edu/~newhall/sysfstutorial.pdf
 * https://www.kernel.org/doc/html/next/filesystems/sysfs.html
 * https://www.kernel.org/doc/html/latest/i2c/writing-clients.html
 * https://www.linux.com/news/how-create-sysfs-file-correctly/
 * GlobalLogic presentation from Pavlo Galkin
 * Lesson 9 - Connecting and using GL Raspberry Kit modules - Kits.ppt
 * https://github.com/Kernel-GL-HRK/gl-kernel-training-2017/tree/master/mpu6050
 * https://devdotnet.org/post/rabota-s-gpio-na-primere-banana-pi-bpi-m64-chast-2-device-tree-overlays/
 * https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/
 */

#include <linux/module.h>
#include <linux/i2c.h>

/* mpu6050 used register's */
#define MPU6050_CONFIG		0x1A
#define MPU6050_GYRO_CONFIG	0x1B
#define MPU6050_ACCEL_CONFIG	0x1C
#define MPU6050_FIFO_EN		0x23
#define MPU6050_INT_PIN_CFG	0x37
#define MPU6050_INT_ENABLE	0x38
#define MPU6050_INT_STATUS	0x3A
#define MPU6050_USER_CTRL	0x6A
#define MPU6050_PWR_MGMT_1	0x6B
#define MPU6050_PWR_MGMT_2	0x6C
#define MPU6050_ACCEL_XOUT_H	0x3B
#define MPU6050_ACCEL_YOUT_H	0x3D
#define MPU6050_ACCEL_ZOUT_H	0x3F
#define MPU6050_GYRO_XOUT_H	0x43
#define MPU6050_GYRO_YOUT_H	0x45
#define MPU6050_GYRO_ZOUT_H	0x47
#define MPU6050_TEMP_OUT_H	0x41
#define MPU6050_TEMP_OUT_L	0x42
#define MPU6050_WHO_AM_I	0x75

/* mpu6050 device id */
#define MPU6050_DEVICE_ID	0x68
/* MPU-6050 GY-521 with RED power LED have DEVICE ID 0x98 */
#define MPU6050_DEVICE_ID_1	0x98

/*
 * An easy way to check a connected gyroscope is to check its ID.
 * The gyro register "WHO_AM_I" - contains 0x68 data.
 */
static int mpu6050_probe(struct i2c_client *client)
{
	int res;
	s32 temp;

	res = i2c_smbus_read_byte_data(client, MPU6050_WHO_AM_I);
	if (IS_ERR_VALUE(res)) {
		pr_debug("mpu6050: i2c device not responding\n");
		return -ENODEV;
	}
	pr_debug("mpu6050: MPU6050_DEVICE_ID = 0x%X\n", res);

	if ((res != MPU6050_DEVICE_ID) && (res != MPU6050_DEVICE_ID_1)) {
		pr_err("mpu6050: Wrong i2c device ID: expected 0x%X, found 0x%X\n",
		       MPU6050_DEVICE_ID, res);
		return -EINVAL;
	}

	/* setup mpu6050 device */
	i2c_smbus_write_byte_data(client, MPU6050_CONFIG, 0);
	i2c_smbus_write_byte_data(client, MPU6050_GYRO_CONFIG, 0);
	i2c_smbus_write_byte_data(client, MPU6050_ACCEL_CONFIG, 0);
	i2c_smbus_write_byte_data(client, MPU6050_FIFO_EN, 0);
	i2c_smbus_write_byte_data(client, MPU6050_INT_PIN_CFG, 0);
	i2c_smbus_write_byte_data(client, MPU6050_INT_ENABLE, 0);
	i2c_smbus_write_byte_data(client, MPU6050_USER_CTRL, 0);
	i2c_smbus_write_byte_data(client, MPU6050_PWR_MGMT_1, 0);
	i2c_smbus_write_byte_data(client, MPU6050_PWR_MGMT_2, 0);

	pr_debug("mpu6050: i2c driver probed\n");

	/* read temperature for test only */
	/* T(C) = temp / 340 + 36.53 = (temp + 12420) / 340 */
	temp = (s16)((u16)(i2c_smbus_read_word_swapped(client, MPU6050_TEMP_OUT_H)));
	pr_debug("mpu6050: temp = %dC\n", (temp + 12420) / 340);

	return 0;
}

static int mpu6050_remove(struct i2c_client *client)
{
	pr_info("mpu6050: i2c driver removed\n");
	return 0;
}

static struct i2c_device_id mpu6050_idtable[] = {
	{ "gl_mpu6050", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, mpu6050_idtable);

static struct i2c_driver mpu6050_driver = {
	.driver = {
		.name	= "gl_mpu6050",
	},

	.id_table	= mpu6050_idtable,
	.probe_new	= mpu6050_probe,
	.remove		= mpu6050_remove,
};

static int __init gl_mpu6050_init(void)
{
	int res;

	/* i2c */
	res = i2c_add_driver(&mpu6050_driver);
	if (res) {
		pr_debug("mpu6050: i2c driver not added\n");
		goto out;
	}
	pr_debug("mpu6050: i2c driver created\n");
	pr_info("mpu6050: module loaded\n");
	return 0;

out:
	pr_err("mpu6050: Driver initialization failed\n");
	return res;
}

static void __exit gl_mpu6050_exit(void)
{
	/* i2c */
	i2c_del_driver(&mpu6050_driver);
	pr_info("mpu6050: module exited\n");
}

module_init(gl_mpu6050_init);
module_exit(gl_mpu6050_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergiy Us <sergiy.us@gmail.com>");
MODULE_DESCRIPTION("A simple gyro mpu6050 kernel device module");
MODULE_VERSION("0.1");

