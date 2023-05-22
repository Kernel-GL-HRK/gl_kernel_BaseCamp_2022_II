#include <linux/init.h>		 // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>	   // Core header for loading LKMs into the kernel
#include <linux/kernel.h>	   // Contains types, macros, functions for the kernel
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/err.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/ioctl.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include "bme280.h"
#include "bme280_defs.h"

#include "temp_ioctl.h"

MODULE_LICENSE("GPL");				  ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Yevhen Kolesnyk");	   ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Character device driver for bme280 with Sys Interface and IOCTL");  ///< The description -- see modinfo
MODULE_VERSION("0.1");				  ///< The version of the module

#define MAX_BUFFER_SIZE 1024
#define SAMPLE_COUNT  UINT8_C(1)


static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev;
dev_t dev;

static int major;
static int is_open;

static size_t buffer_size;

static struct timer_list  etx_timer;

static uint32_t period;
struct bme280_settings settings;

static struct i2c_client *bme280_client;
static struct bme280_dev bme280;
static struct bme280_data comp_data;

static int8_t bme280_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
	int8_t ret = 0;
	uint32_t i;

	for (i = 0; i < len; i++) {
		ret = i2c_smbus_write_byte_data(bme280_client, reg_addr + i, reg_data[i]);
		if (ret < 0)
			break;
	}

	return ret;

}

static int8_t bme280_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
	int8_t ret = 0;
	uint32_t i;

	for (i = 0; i < len; i++) {
		reg_data[i] = i2c_smbus_read_byte_data(bme280_client, reg_addr + i);
		if (reg_data[i] < 0) {
			ret = -1;
			break;
		}
	}

	return ret;


}

static void bme280_delay_us(uint32_t period, void *intf_ptr)
{
	udelay(period);
}


static int bme280_device_init(void)
{

	uint8_t rslt = 0;

	bme280.chip_id = BME280_I2C_ADDR_PRIM; // Задайте адрес I2C датчика
	bme280.intf = BME280_I2C_INTF; // Установите интерфейс I2C
	bme280.read = bme280_read; // Задайте функцию чтения I2C для вашего драйвера
	bme280.write = bme280_write; // Задайте функцию записи I2C для вашего драйвера
	bme280.delay_us = bme280_delay_us; // Задайте функцию задержки для вашего драйвера

	rslt = (bme280_init(&bme280) != BME280_OK);
	if (rslt) {
		pr_err("bme280: ERR bme280_init %d\n", rslt);
		return -ENODEV;
	}

	rslt = bme280_get_sensor_settings(&settings, &bme280);
	if (rslt) {
		pr_err("bme280: ERR bme280_get_sensor_settings %d\n", rslt);
		return -ENODEV;
	}

		/* Configuring the over-sampling rate, filter coefficient and standby time */
		/* Overwrite the desired settings */
	settings.filter = BME280_FILTER_COEFF_2;

		/* Over-sampling rate for humidity, temperature and pressure */
	settings.osr_h = BME280_OVERSAMPLING_1X;
	settings.osr_p = BME280_OVERSAMPLING_1X;
	settings.osr_t = BME280_OVERSAMPLING_1X;

		/* Setting the standby time */
	settings.standby_time = BME280_STANDBY_TIME_0_5_MS;

	rslt = bme280_set_sensor_settings(BME280_SEL_ALL_SETTINGS, &settings, &bme280);
	if (rslt) {
		pr_err("bme280: ERR bme280_set_sensor_settings %d\n", rslt);
		return -ENODEV;
	}

	rslt = bme280_set_sensor_mode(BME280_POWERMODE_NORMAL, &bme280);
	if (rslt) {
		pr_err("bme280: ERR bme280_set_sensor_mode %d\n", rslt);
		return -ENODEV;
	}

	rslt = bme280_cal_meas_delay(&period, &settings);
	if (rslt) {
		pr_err("bme280: ERR bme280_cal_meas_delay %d\n", rslt);
		return -ENODEV;
	}


	return rslt;
}

static int8_t get_temperature(uint32_t period, struct bme280_dev *dev)
{
	int8_t rslt = BME280_E_NULL_PTR;
	int8_t idx = 0;
	uint8_t status_reg;


	while (idx < SAMPLE_COUNT) {
		rslt = bme280_get_regs(BME280_REG_STATUS, &status_reg, 1, dev);
		if (rslt) {
			pr_err("bme280: ERR bme280_get_regs %d\n", rslt);
			return rslt;
		}


		if (status_reg & BME280_STATUS_MEAS_DONE) {
			/* Measurement time delay given to read sample */
			dev->delay_us(period, dev->intf_ptr);

			/* Read compensated data */
			rslt = bme280_get_sensor_data(BME280_TEMP, &comp_data, dev);

			if (rslt)
				pr_err("bme280: ERR bme280_get_sensor_data\n");

			//pr_info("bme280: Temperature[%d]:  %ld deg C\n", idx, (long int)comp_data.temperature);

			idx++;
		}
	}

	return rslt;
}


static ssize_t temperature_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("bme280: sysfs show %s", __func__);
	return sprintf(buf, "%ld\n", (long)comp_data.temperature);
}

static struct kobject *chrdev_kobj;

struct kobj_attribute temperature_attr = __ATTR(chr_temperature, 0660, temperature_show, 0);


static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("bme280: already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_info("bme280: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("bme280: device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	char temp_value[16];

	ssize_t num_bytes;

	if (get_temperature(period, &bme280))
		pr_err("bme280: ERR get_temperature");

	pr_info("bme280: read from file %s\n", filep->f_path.dentry->d_iname);
	pr_info("bme280: read from device %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));

	snprintf(temp_value, sizeof(temp_value), "%d\n", comp_data.temperature);
	num_bytes = strlen(temp_value);

	if (len < num_bytes) {
		pr_err("bme280: copy to user failed\n");
		return -EINVAL;
	}


	if (copy_to_user(buffer, temp_value, num_bytes))
		return -EFAULT;

	return num_bytes;
}


static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int value = 0;

	pr_info("bme280:  %s() cmd=0x%x arg=0x%lx\n", __func__, cmd, arg);

	if (_IOC_TYPE(cmd) != CDEV_IOC_MAGIC) {
		pr_err("bme280: CHARDEV_IOC_MAGIC error\n");
		err = -ENOTTY;
	}

	if (_IOC_NR(cmd) >= TEMP_IOC_MAXNR) {
		pr_err("bme280:  TEMP_IOC_MAXNR err\n");
		err = -ENOTTY;
	}

	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (err) {
		pr_err("bme280:  _IOC_READ/_IOC_WRITE err\n");
		err = -EFAULT;
	}

	switch (_IOC_NR(cmd)) {
	case GET_TEMP:
		if (get_temperature(period, &bme280))
			pr_err("bme280: ERR get_temperature");

		if (copy_to_user((int32_t *)arg, &comp_data.temperature, sizeof(comp_data.temperature))) {
			pr_err("bme280:  GET_ENCODER err\n");
			err = -ENOTTY;
		}
		pr_info("bme280:  GET_ENCODER temperature=%d\n", value);
		break;
	default:
		pr_warn("bme280:  invalid cmd(%u)\n", cmd);
	}

	return err;
}


static struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.unlocked_ioctl = dev_ioctl,
};


static int bme280_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

	int tmp, client_id;
	struct i2c_adapter *adapter;

	is_open = 0;
	buffer_size = 0;
	dev = 0;
	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);

	/* Check if adapter supports the functionality we need */
	adapter = client->adapter;
	tmp = i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA);
	if (!tmp) {

		pr_err("bme280: i2c_check_functionality failed:");
		goto err_out;

	}

	/* Get chip_id */
	client_id = i2c_smbus_read_byte_data(client, R_BME280_CHIP_ID);
	if (client_id != BME280_CHIP_ID) {
		pr_err("bme280: Client ID (%x) does not match chip ID (%x).\n", client_id, BME280_CHIP_ID);
		goto err_out;
	}
	bme280_client = client;


	if (major < 0) {
		pr_err("bme280: register_chrdev failed: %d\n", major);
		return major;
	}
	pr_info("bme280: register_chrdev ok, major = %d minor = %d\n", MAJOR(dev), MINOR(dev));
	cdev_init(&chrdev_cdev, &fops);

	if ((cdev_add(&chrdev_cdev, dev, 1)) < 0) {
		pr_err("bme280: cannot add the device to the system\n");
		goto cdev_err;
	}
	pr_info("bme280: cdev created successfully\n");

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass))
		goto class_err;

	pr_info("bme280: device class created successfully\n");

	pdev = device_create(pclass, NULL, dev, NULL, CLASS_NAME"0");

	if (IS_ERR(pdev))
		goto device_err;

	pr_info("bme280: device created successfully\n");

	chrdev_kobj = kobject_create_and_add("bme280_sysfs", kernel_kobj);

		if (sysfs_create_file(chrdev_kobj, &temperature_attr.attr)) {
			pr_err("bme280: cannot create sysfs file\n");
			goto temperature_err;
	}

	pr_info("bme280: device driver insmod success\n");


	if (bme280_device_init())
		pr_err("bme280: bme280_device_init failed\n");
	else
		pr_info("bme280: period is %d\n", period);


	pr_info("bme280: %s()\n", __func__);

	return 0;

temperature_err:
	kobject_put(chrdev_kobj);
device_err:
	device_destroy(pclass, dev);
class_err:
	class_destroy(pclass);
cdev_err:
	unregister_chrdev_region(dev, 1);
err_out:

	return -EFAULT;

}

static int bme280_remove(struct i2c_client *client)
{
	del_timer(&etx_timer);
	kobject_put(chrdev_kobj);
	sysfs_remove_file(kernel_kobj, &temperature_attr.attr);

	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("bme280: module exited\n");

	return 0;

}

static struct i2c_device_id bme280_idtable[] = {
	{DEVICE_NAME, 0},
	{ }
};
MODULE_DEVICE_TABLE(i2c, bme280_idtable);



static struct i2c_driver bme280_driver = {
	.driver = {
			.name = DEVICE_NAME,
			.owner = THIS_MODULE,
		  },
	.probe = bme280_probe,
	.remove = bme280_remove,
	.id_table = bme280_idtable
};




static int __init chrdev_init(void)
{
	if (i2c_add_driver(&bme280_driver))
		pr_err("bme280: I2C subsystem registration failed.\n");
	pr_info("bme280: Device successfully registered to I2C subsystem.\n");
	return tmp;
}

static void __exit chrdev_exit(void)
{
/* Unregister driver from I2C subsystem */
	i2c_del_driver(&bme280_driver);
}

module_init(chrdev_init);
module_exit(chrdev_exit);
