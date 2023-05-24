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
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#define SYSFS_DIR		"mpu6050"
#define SYSFS_ACCEL		"accel"
#define SYSFS_GYRO		"gyro"
#define SYSFS_TEMP		"temp"
#define ACCEL_X			accel_x
#define ACCEL_Y			accel_y
#define ACCEL_Z			accel_z
#define GYRO_X			gyro_x
#define GYRO_Y			gyro_y
#define GYRO_Z			gyro_z
#define TEMP			temp
#define DIRECT			direct

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

/* data structure for gyro */
static struct gyro {
	struct i2c_client *drv_client;
	s32 temperature;
	s32 accel[3];
	s32 gyro[3];
} gyro;

static struct gyro *mpu6050_data = &gyro;
static struct device *dev;
static struct kobject *root_kobj;
static struct kobject *accel_kobj;
static struct kobject *gyro_kobj;

/**
 * mpu6050_get_raw - getting raw data from the gyroscope.
 * @:	No input argument.
 *
 * This function reads accelerometer (x, y, z), gyroscope (x, y, z) and
 * temperature data. Accelerometer and gyroscope data are stored in the
 * mpu6050_data structure in raw form. The raw temperature data is
 * converted to degrees Celsius using the formula temp/340 + 36.53 and
 * stored in the mpu6050_data structure.
 *
 * Retrun:
 * 0 - OK
 * -ENODEV - Device not found or not responding.
 */
static int mpu6050_get_raw(void)
{
	s32 temp;
	struct i2c_client *drv = mpu6050_data->drv_client;

	if (drv == 0) {
		dev_err(&drv->dev, "mpu6050 i2c client fail\n");
		return -ENODEV;
	}

	/* read accel */
	mpu6050_data->accel[0] = (s16)((u16)(i2c_smbus_read_word_swapped(
		drv, MPU6050_ACCEL_XOUT_H)));
	mpu6050_data->accel[1] = (s16)((u16)(i2c_smbus_read_word_swapped(
		drv, MPU6050_ACCEL_YOUT_H)));
	mpu6050_data->accel[2] = (s16)((u16)(i2c_smbus_read_word_swapped(
		drv, MPU6050_ACCEL_ZOUT_H)));

	/* read gyro */
	mpu6050_data->gyro[0] = (s16)((u16)(i2c_smbus_read_word_swapped(
		drv, MPU6050_GYRO_XOUT_H)));
	mpu6050_data->gyro[1] = (s16)((u16)(i2c_smbus_read_word_swapped(
		drv, MPU6050_GYRO_YOUT_H)));
	mpu6050_data->gyro[2] = (s16)((u16)(i2c_smbus_read_word_swapped(
		drv, MPU6050_GYRO_ZOUT_H)));

	/* read temperature */
	/* T(c) = temp / 340 + 36.53 = (temp + 12420) / 340 */
	temp = (s16)((u16)(i2c_smbus_read_word_swapped(drv, MPU6050_TEMP_OUT_H)));
	mpu6050_data->temperature = (temp + 12420) / 340;

	return 0;
}

static ssize_t accel_x_show(struct device *dev,
			    struct device_attribute *attr,
			    char *buf)
{
	int res;

	res = mpu6050_get_raw();
	if (res) {
		pr_err("mpu6050: Error reading accel x data from mpu6050\n");
		return res;
	}
	sprintf(buf, "%d\n", mpu6050_data->accel[0]);
	return strlen(buf);
}

static ssize_t accel_y_show(struct device *dev,
			    struct device_attribute *attr,
			    char *buf)
{
	int res;

	res = mpu6050_get_raw();
	if (res) {
		pr_err("mpu6050: Error reading accel y data from mpu6050\n");
		return res;
	}
	sprintf(buf, "%d\n", mpu6050_data->accel[1]);
	return strlen(buf);
}

static ssize_t accel_z_show(struct device *dev,
			    struct device_attribute *attr,
			    char *buf)
{
	int res;

	res = mpu6050_get_raw();
	if (res) {
		pr_err("mpu6050: Error reading accel z data from mpu6050\n");
		return res;
	}
	sprintf(buf, "%d\n", mpu6050_data->accel[2]);
	return strlen(buf);
}

static ssize_t gyro_x_show(struct device *dev,
			   struct device_attribute *attr,
			   char *buf)
{
	int res;

	res = mpu6050_get_raw();
	if (res) {
		pr_err("mpu6050: Error reading gyro x data from mpu6050\n");
		return res;
	}
	sprintf(buf, "%d\n", mpu6050_data->gyro[0]);
	return strlen(buf);
}

static ssize_t gyro_y_show(struct device *dev,
			   struct device_attribute *attr,
			   char *buf)
{
	int res;

	res = mpu6050_get_raw();
	if (res) {
		pr_err("mpu6050: Error reading gyro y data from mpu6050\n");
		return res;
	}
	sprintf(buf, "%d\n", mpu6050_data->gyro[1]);
	return strlen(buf);
}

static ssize_t gyro_z_show(struct device *dev,
			   struct device_attribute *attr,
			   char *buf)
{
	int res;

	res = mpu6050_get_raw();
	if (res) {
		pr_err("mpu6050: Error reading gyro z data from mpu6050\n");
		return res;
	}
	sprintf(buf, "%d\n", mpu6050_data->gyro[2]);
	return strlen(buf);
}

static ssize_t temp_show(struct device *dev,
			 struct device_attribute *attr,
			 char *buf)
{
	int res;

	res = mpu6050_get_raw();
	if (res != 0) {
		pr_err("mpu6050: Error reading temperature from mpu6050\n");
		return res;
	}
	sprintf(buf, "%d\n", mpu6050_data->temperature);
	return strlen(buf);
}

/**
 * direct_show - sysfs show function to get direction.
 * @: standard input argument for sysfs show.
 *
 * The function uses a simple algorithm for determining the position
 * by two coordinates (x, y).
 * We use n = SAMPLES accelerometer measurements with a delay ms.
 * Then we get the average value for each coordinate.
 * If the value of the [x] coordinate is less than the threshold value
 * (SENSETIVE), we assume that it is a forward [F] movement, if it is
 * more - backward , similarly for the [y] coordinate - if it is less,
 * then it is a movement to the left, if more - to the right.
 * Otherwise, movements in the coordinates [x, y] are not recorded - [0].
 *
 *		2[FORWARD]
 *	4[LEFT]		6[RIGHT]
 *		8[BACK]
 *
 * Return:
 * bufer size	- OK
 * -ENODEV	- No such device.
 */
static ssize_t direct_show(struct device *dev,
			   struct device_attribute *attr,
			   char *buf)
{
#define SAMPLES		(16)
#define SENSETIVE	(8000)
#define DELAY_MS	(1)
#define NODIRECT	(0)
#define FORWARD		(2)
#define	BACK		(8)
#define	LEFT		(4)
#define RIGHT		(6)

	int res;
	u32 direction;
	size_t i;
	s32 average[3] = {0, 0, 0};

	for (i = 0; i < SAMPLES; ++i) {
		res = mpu6050_get_raw();
		if (res) {
			pr_err("mpu6050: Error reading data from mpu6050!\n");
			return -ENODEV;
		}
		/* save accel_x/y/z */
		average[0] += mpu6050_data->accel[0];
		average[1] += mpu6050_data->accel[1];
		average[2] += mpu6050_data->accel[2];
		msleep(DELAY_MS);
	}

	average[0] /= SAMPLES;
	average[1] /= SAMPLES;

	if (average[0] < -SENSETIVE) {
		direction = FORWARD;
	} else if (average[0] > SENSETIVE) {
		direction = BACK;
	} else if (average[1] < -SENSETIVE) {
		direction = LEFT;
	} else if (average[1] > SENSETIVE) {
		direction = RIGHT;
	} else {
		direction = NODIRECT;
	}

	sprintf(buf, "%d\n", direction);
	return strlen(buf);
}

/**
 * struct device_attribute - mpu6050 sysfs device attributes.
 * @NAME: attribure name.
 * @0444: permissions (all read).
 * @show: show function.
 */
static struct device_attribute accel_x_attr = __ATTR(ACCEL_X, 0444,
						     accel_x_show, NULL);
static struct device_attribute accel_y_attr = __ATTR(ACCEL_Y, 0444,
						     accel_y_show, NULL);
static struct device_attribute accel_z_attr = __ATTR(ACCEL_Z, 0444,
						     accel_z_show, NULL);
static struct device_attribute gyro_x_attr  = __ATTR(GYRO_X, 0444,
						     gyro_x_show, NULL);
static struct device_attribute gyro_y_attr  = __ATTR(GYRO_Y, 0444,
						     gyro_y_show, NULL);
static struct device_attribute gyro_z_attr  = __ATTR(GYRO_Z, 0444,
						     gyro_z_show, NULL);
static struct device_attribute temp_attr    = __ATTR(TEMP, 0444,
						     temp_show, NULL);
static struct device_attribute direct_attr  = __ATTR(DIRECT, 0444,
						     direct_show, NULL);

/**
 * mpu6050_probe - probe mpu6050 device presence.
 * @*client - struct i2c_client.
 *
 * An easy way to check a connected gyroscope is to check its ID.
 * The gyro register "WHO_AM_I" - contains 0x68 data.
 * But some mpu6050 GY-521 gyro modules wit RED power led have
 * DEVICE ID - 0x98!
 *
 * Return:
 * 0 - OK
 * -ENODEV - Device not responding
 * -ENIVAL - Wrong device ID.
 */
static int mpu6050_probe(struct i2c_client *client)
{
	int res;

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

	/* save i2c_client */
	mpu6050_data->drv_client = client;
	pr_debug("mpu6050: i2c driver probed\n");
	return 0;
}

static int mpu6050_remove(struct i2c_client *client)
{
	mpu6050_data->drv_client = 0;
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
		.owner	= THIS_MODULE,
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

	/* sysfs */
	/* Create /sys/devices/SYSFS_DIR */
	dev = root_device_register(SYSFS_DIR);
	root_kobj = &dev->kobj;

	/* Create /sys/devices/SYSFS_DIR/SYSFS_ACCEL/ */
	accel_kobj = kobject_create_and_add(SYSFS_ACCEL, root_kobj);

	/* Create sysfs file /sys/devices/SYSFS_DIR/SYSFS_ACCEL/accel_x */
	if (sysfs_create_file(accel_kobj, &accel_x_attr.attr))
		goto out_unreg_i2c;

	/* Create sysfs file /sys/devices/SYSFS_DIR/SYSFS_ACCEL/accel_y */
	if (sysfs_create_file(accel_kobj, &accel_y_attr.attr))
		goto out_unreg_accel_x;

	/* Create sysfs file /sys/devices/SYSFS_DIR/SYSFS_ACCEL/accel_z */
	if (sysfs_create_file(accel_kobj, &accel_z_attr.attr))
		goto out_unreg_accel_y;

	/* Create /sys/devices/SYSFS_DIR/SYSFS_GYRO/ */
	gyro_kobj = kobject_create_and_add(SYSFS_GYRO, root_kobj);

	/* Create sysfs file /sys/devices/SYSFS_DIR/SYSFS_GYRO/gyro_x */
	if (sysfs_create_file(gyro_kobj, &gyro_x_attr.attr))
		goto out_unreg_accel_z;

	/* Create sysfs file /sys/devices/SYSFS_DIR/SYSFS_GYRO/gyro_y */
	if (sysfs_create_file(gyro_kobj, &gyro_y_attr.attr))
		goto out_unreg_gyro_x;

	/* Create sysfs file /sys/devices/SYSFS_DIR/SYSFS_GYRO/gyro_z */
	if (sysfs_create_file(gyro_kobj, &gyro_z_attr.attr))
		goto out_unreg_gyro_y;

	/* Create sysfs file /sys/devices/TEMP */
	if (sysfs_create_file(root_kobj, &temp_attr.attr))
		goto out_unreg_gyro_z;

	/* Create sysfs file for direction /sys/devices/DIRECT */
	if (sysfs_create_file(root_kobj, &direct_attr.attr))
		goto out_unreg_temp;

	pr_info("mpu6050: module loaded\n");
	return 0;

out_unreg_temp:
	sysfs_remove_file(root_kobj, &temp_attr.attr);
out_unreg_gyro_z:
	sysfs_remove_file(gyro_kobj, &gyro_z_attr.attr);
out_unreg_gyro_y:
	sysfs_remove_file(gyro_kobj, &gyro_y_attr.attr);
out_unreg_gyro_x:
	sysfs_remove_file(gyro_kobj, &gyro_x_attr.attr);
out_unreg_accel_z:
	kobject_put(gyro_kobj);
	sysfs_remove_file(accel_kobj, &accel_z_attr.attr);
out_unreg_accel_y:
	sysfs_remove_file(accel_kobj, &accel_y_attr.attr);
out_unreg_accel_x:
	sysfs_remove_file(accel_kobj, &accel_x_attr.attr);
out_unreg_i2c:
	kobject_put(accel_kobj);
	root_device_unregister(dev);
	i2c_del_driver(&mpu6050_driver);
out:
	pr_err("mpu6050: Driver initialization failed\n");
	return res;
}

static void __exit gl_mpu6050_exit(void)
{
	/* sysfs */
	sysfs_remove_file(root_kobj, &direct_attr.attr);
	sysfs_remove_file(root_kobj, &temp_attr.attr);
	sysfs_remove_file(gyro_kobj, &gyro_z_attr.attr);
	sysfs_remove_file(gyro_kobj, &gyro_y_attr.attr);
	sysfs_remove_file(gyro_kobj, &gyro_x_attr.attr);
	kobject_put(gyro_kobj);
	sysfs_remove_file(accel_kobj, &accel_z_attr.attr);
	sysfs_remove_file(accel_kobj, &accel_y_attr.attr);
	sysfs_remove_file(accel_kobj, &accel_x_attr.attr);
	kobject_put(accel_kobj);
	root_device_unregister(dev);

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

