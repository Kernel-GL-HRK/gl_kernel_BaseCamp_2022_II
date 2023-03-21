/*
 *  @file        temp_led_module.ko
 *  @author      Mykhailo Vyshnevskyi
 *  @date        10 March 2023
 *  @version     0.2.0
 *  @details     A driver for measuring CPU temperature using the \"cpu-thermal\"
 *               sensor and indicating temperature using three LEDs.
 *  Work algorithm:
 *               When starting the driver, all LEDs are on for 5 seconds.
 *               The temperature measurement interval is 5 seconds.
 *               A GREEN LED blinks when the temperature is below 40 degrees,
 *               a YELLOW LED blinks when the temperature is below 60 degrees, and
 *               a RED LED blinks when the temperature is below 75 degrees.
 *               All LEDs remain on, and the RED LED blinking when the temperature is above 75 degrees.
 *               Additionally, the driver (proc/chrdev_proc) responds to the command
 *                        "cat /proc/chrdev_proc/chrdev_temp_blink_proc"
 *               and outputs the message:
 *                          "temperature = %d.%d Grad"
 *							"Led Green < %d Grad"
 *							"Led Yellow < %d Grad"
 *							"Led Red < %d Grad"
 *							"Leds All > %d Grad"
 *
 *  variables to define temperature limits in 3 ranges:
 *				 		to_temp_green = 40000; // Green will be lit up until this temperature
 *				 		to_temp_yellow = 60000; // Yellow will be lit up until this temperature.
 *				 		to_temp_red = 75000; // Red will be lit up until this temperature
 *				 for read and write variables,
 *				 example:
 *				 		/sys/kernel/chrdev_temp_blink$ sudo su // write only under super user (permission=0664)
 *				 		/sys/kernel/chrdev_temp_blink# echo 31000 > to_temp_green
 * 				 		/sys/kernel/chrdev_temp_blink$ cat to_temp_green
 *				 		31000
 *						input condition: to_temp_green < to_temp_yellow < to_temp_red
 *	working with the buffer in /dev/chrdev0
 *				 example:
 *				 /dev$ sudo su // write only under super user
 *				 /dev# echo test messeg 123 > chrdev0
 *				 /dev$ cat chrdev_temp_blink0
 *				 test messeg 123
 *				 /dev$dmesg  // output the device number (Major, Minor) and the size of the transmitted information in bytes
 *
 *  GPIO:
 *               GPIO_5     RED
 *               GPIO_6     YELLOW
 *               GPIO_26    GREEN
 *
 * 	Tested with Linux raspberrypi [5.10.103+]
 */

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>  // device fs
#include <linux/thermal.h> // Thermal Zone  Temp
#include <linux/sysfs.h>
#include <linux/proc_fs.h> //
#include <linux/cdev.h>	   // character device driver
#include <linux/kdev_t.h>
#include <linux/kobject.h> //Kernel Objects
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/gpio.h> //GPIO
#include <linux/err.h>
#include <linux/ioctl.h>

#include "chrdev_ioctl.h"
#include "temp_led_plus_module.h"

// /sys  /dev 
static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev;
dev_t dev = 0; // Major Minor

static int major;
static int is_open;

static int data_size;
static unsigned char data_buffer[BUFFER_SIZE];

data_buffer_info_t q; // ioctl struc
//static int32_t ioctl_val;

static int len_proc = 0; // Length of procfs_buffer

// Buffer and size for the proc file
static char procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t procfs_buffer_size;
/* end procfs */

// Thermal zone device for temperature readings
struct thermal_zone_device *tz;
static int temp;
static int to_temp_buf = 0;
static int to_temp_green = 40000;  // Green will be lit up until this temperature
static int to_temp_yellow = 60000; // Yellow will be lit up until this temperature
static int to_temp_red = 75000;	   // Red will be lit up until this temperature

// static int gpio_index; // 0, 1, 2

// Timer
static struct timer_list timer_blink, timer_thermal;
static bool flag_timer;

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

// Timer callback function for blinking LEDs
void timer_blink_callback(struct timer_list *data)
{
	if (flag_timer == false)
	{
		gpio_set_value(led_array[gpio_index].gpio, 0);
		mod_timer(&timer_blink, jiffies + msecs_to_jiffies(10));
		// pr_info("led_array[gpio_index= %d].gpio = %d\n", gpio_index, led_array[gpio_index].gpio);
	}
	else
	{
		gpio_set_value(led_array[gpio_index].gpio, 1);
		mod_timer(&timer_blink, jiffies + msecs_to_jiffies(1));
	}
	flag_timer = !flag_timer;
}

// Callback function for thermal events
void thermal_callback(struct timer_list *data)
{
	if (thermal_zone_get_temp(tz, &temp))
	{
		pr_err("%s Failed to get temperature\n", __func__);
	}
	// pr_info("Temperature: %d\n", temp);
	if (temp < to_temp_green)
	{ // LED GREEN blinklicht
		gpio_index = 2;
		gpio_set_value(GPIO_5, 0);
		gpio_set_value(GPIO_6, 0);
		gpio_set_value(GPIO_26, 1);
	}
	else if (temp < to_temp_yellow)
	{ // LED YELLOW blinklicht
		gpio_index = 1;
		gpio_set_value(GPIO_5, 0);
		gpio_set_value(GPIO_6, 1);
		gpio_set_value(GPIO_26, 0);
	}
	else if (temp < to_temp_red)
	{ // LED RED blinklicht
		gpio_index = 0;
		gpio_set_value(GPIO_5, 1);
		gpio_set_value(GPIO_6, 0);
		gpio_set_value(GPIO_26, 0);
	}
	else
	{ // LED RED blinklicht, YELLOW und GREEN licht
		gpio_index = 0;
		gpio_set_value(GPIO_5, 1);
		gpio_set_value(GPIO_6, 1);
		gpio_set_value(GPIO_26, 1);
	}
	mod_timer(&timer_thermal, jiffies + msecs_to_jiffies(TIMEOUT));
}

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open)
	{
		pr_err("chrdev: already open\n");
		return -EBUSY;
	}
	is_open = 1;
	pr_info("chrdev: device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_info("chrdev: device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	int ret;

	pr_info("chrdev: read from file %s\n", filep->f_path.dentry->d_name.name); // .dentry->d_iname
	pr_info("chrdev: read from device %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));

	if (len > data_size)
		len = data_size;

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret)
	{
		pr_err("chrdev: copy_to_user failed: %d\n", ret);
		return -EFAULT;
	}
	data_size = 0; /* eof for cat */

	pr_info("chrdev: %zu bytes read\n", len);
	return len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	int ret;

	pr_info("chrdev0: write to file %s\n", filep->f_path.dentry->d_name.name); // filep->f_inode.dentry->d_iname
	pr_info("chrdev0: write to device %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));
	data_size = len;
	if (data_size > BUFFER_SIZE)
		data_size = BUFFER_SIZE;

	ret = copy_from_user(data_buffer, buffer, data_size);
	if (ret)
	{
		pr_err("chrdev: copy_from_user failed: %d\n", ret);
		return -EFAULT;
	}
	pr_info("chrdev0: __data_buffer__ = %s", data_buffer);
	pr_info("chrdev0: %d bytes written\n", data_size);
	return data_size;
}

// ioctl
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	pr_info("chrdev: %s() cmd=0x%x arg=0x%lx\n", __func__, cmd, arg);

	if(_IOC_TYPE(cmd) != CDEV_IOC_MAGIC)
	{
		pr_err("chrdev: CDEV_IOC_MAGIC err!\n");
		return -ENOTTY;
	}

	if (_IOC_NR(cmd) >= CDEV_IOC_MAXNR)
	{
		pr_err("chrdev: CDEV_IOC_MAXNR err!\n");
		return -ENOTTY;
	}

	if (_IOC_DIR(cmd) & _IOC_READ)
	{
		err = ! access_ok((void __user *)arg, _IOC_SIZE(cmd));
	}

	if (_IOC_DIR(cmd) & _IOC_WRITE)
	{
		err = ! access_ok((void __user *)arg, _IOC_SIZE(cmd));
	}

	if (err)
	{
		pr_err("chrdev: _IOC_READ / _IOC_WRITE err! = %d\n", err);
		return -EFAULT;
	}

	switch (cmd)
	{
	case CDEV_WR_VALUE:
		if (copy_from_user(&q, (data_buffer_info_t *)arg, sizeof(data_buffer_info_t)))
		{
			pr_err("chrdev: data write : Err!\n");
		}
		pr_info("chrdev WR_VALUE q.ioctl_from_user_value = %d\n", q.ioctl_from_user_value);

		break;
	case CDEV_RD_VALUE:
		q.ioctl_to_user_value = q.ioctl_from_user_value;
		if (copy_to_user((data_buffer_info_t *)arg, &q, sizeof(data_buffer_info_t)))
		{
			pr_err("chrdev: data read : Err!\n");
		}
		pr_info("chrdev RD_VALUE q.ioctl_to_user_value = %d\n", q.ioctl_to_user_value);
		break;
	case CDEV_CLEAR_BUFFER:
		memset(data_buffer, '\0', 1);
		pr_info("chrdev CLEAR_BUFFER data_buffer = %s\n", data_buffer);
		break;
	case CDEV_GET_BUFFER_SIZE:
		q.ioctl_arr_size_val = sizeof(data_buffer);
		// q.ioctl_str_size_val = strlen(data_buffer);
		q.ioctl_str_size_val = data_size;
		// snprintf(q.ioctl_data_buffer, sizeof(q.ioctl_data_buffer), "%s", data_buffer);

		if (copy_to_user((data_buffer_info_t *)arg, &q, sizeof(data_buffer_info_t)))
		{
			pr_err("chrdev: CDEV_GET_BUFFER_SIZE data read : Err!\n");
		}
		pr_info("chrdev CDEV_GET_BUFFER_SIZE data_buffer[BUFFER_SIZE] array size is %d Byte\n", q.ioctl_arr_size_val);
		pr_info("chrdev CDEV_GET_BUFFER_SIZE the size of the string including the character '0' = %d Byte\n", q.ioctl_str_size_val);
		break;
	case CDEV_WRRD_ARR_VALUE:
		if (copy_from_user(&q, (data_buffer_info_t *)arg, sizeof(data_buffer_info_t)))
		{
			pr_err("chrdev: data write : Err!\n");
		}
		pr_info("chrdev WR_VALUE q.ioctl_from_user_value = %d\n", q.ioctl_from_user_value);
		memcpy(q.ioctl_data_buffer, data_buffer, q.ioctl_from_user_value);
		
		if (copy_to_user((data_buffer_info_t *)arg, &q, sizeof(data_buffer_info_t)))
		{
			pr_err("chrdev: CDEV_GET_BUFFER_SIZE data read : Err!\n");
		}
		q.ioctl_to_user_value = 0; // copy into User Space can
		pr_info("chrdev CDEV_WRRD_ARR_VALUE data_buffer[%d] = %.*s\n", q.ioctl_from_user_value, q.ioctl_from_user_value, q.ioctl_data_buffer);
		break;
	default:
		pr_warn("chrdev: invalid cmd(%u)\n", cmd);
		break;
	}
	return 0;
}

static struct file_operations fops =
{
		.open = dev_open,
		.release = dev_release,
		.read = dev_read,
		.write = dev_write,
		.unlocked_ioctl = dev_ioctl,
};

/* procfs */
static ssize_t info_temp_proc(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
	// Get the temperature from the thermal zone
	if (thermal_zone_get_temp(tz, &temp))
	{
		pr_err("Failed to get temperature\n");
	}

	// Determine the size of the buffer to be read
	procfs_buffer_size = min(count, (size_t)PROC_BUFFER_SIZE);

	// If the current position is greater than or equal to the buffer size, there's nothing more to read
	if (*pos >= procfs_buffer_size)
	{
		len_proc = 0;
		return 0;
	}

	// Format the temperature string to be written to the buffer
	sprintf(procfs_buffer, "\nCPU temperature = %d.%d Grad\n", temp / 1000, temp % 1000);
	len_proc += strlen(procfs_buffer);
	sprintf(procfs_buffer + strlen(procfs_buffer), "\nLed Green  < %d/1000 Grad\n"
												   "Led Yellow < %d/1000 Grad\n"
												   "Led Red    < %d/1000 Grad\n"
												   "Leds All   > %d/1000 Grad\n"
												   "\nranges can be changed via \n/sys/kernel/chrdev_temp_blink$\n"
												   "only with SUPER USER\ninput format from 1 to 90000\nexample:\n"
												   "/sys/kernel/chrdev_temp_blink# echo 31000 > to_temp_green  (31 Grad)\n\n",
			to_temp_green,
			to_temp_yellow,
			to_temp_red,
			to_temp_red);

	// If reading the buffer would cause a buffer overflow, adjust the size of the buffer accordingly
	if (*pos + procfs_buffer_size > PROC_BUFFER_SIZE)
	{
		procfs_buffer_size = PROC_BUFFER_SIZE - *pos;
	}

	len_proc += strlen(procfs_buffer);
	pr_info(" ______Length of procfs_buffer: %zu\n", len_proc);

	// Copy the buffer contents to user space
	if (copy_to_user(buf, procfs_buffer + *pos, procfs_buffer_size))
	{
		return -EFAULT;
	}

	// Update the current position in the buffer
	*pos += procfs_buffer_size;

	// Return the number of bytes read
	return procfs_buffer_size;
}

// Create procfs file for temperature
static const struct proc_ops read_temp_fops = {
	.proc_read = info_temp_proc,
};
/* end procfs */
/*************** Sysfs functions **********************/
unsigned int sysfs_val = 0;

// This function will be called when we read the sysfs file
static ssize_t sysfs_green_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("Reading - sysfs show func... to_temp_green = %d\n", to_temp_green);
	return sprintf(buf, "%d\n", to_temp_green);
}
// This function will be called when we write the sysfs file
static ssize_t sysfs_green_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	pr_info("Writing - sysfs store function to_temp_green...\n");
	sscanf(buf, "%d", &to_temp_buf);
	if (to_temp_buf < to_temp_yellow)
	{
		to_temp_green = to_temp_buf;
	}
	else
	{
		pr_err("Error writing to sysfs, should be TO_TEMP_GREEN < to_temp_yellow < to_temp_red\n");
	}
	return count;
}

static ssize_t sysfs_yellow_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("Reading - sysfs show func... to_temp_yellow = %d\n", to_temp_yellow);
	return sprintf(buf, "%d\n", to_temp_yellow);
}
static ssize_t sysfs_yellow_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	pr_info("Writing - sysfs store function to_temp_yellow...\n");
	sscanf(buf, "%d", &to_temp_buf);
	if (to_temp_green < to_temp_buf && to_temp_buf < to_temp_red)
	{
		to_temp_yellow = to_temp_buf;
	}
	else
	{
		pr_err("Error writing to sysfs, should be to_temp_green < TO_TEMP_YELLOW < to_temp_red\n");
	}
	return count;
}
static ssize_t sysfs_red_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("Reading - sysfs show func... to_temp_red = %d\n", to_temp_red);
	return sprintf(buf, "%d\n", to_temp_red);
}
static ssize_t sysfs_red_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	pr_info("Writing - sysfs store function to_temp_red...\n");
	sscanf(buf, "%d", &to_temp_buf);
	if (to_temp_yellow < to_temp_buf)
	{
		to_temp_red = to_temp_buf;
	}
	else
	{
		pr_err("Error writing to sysfs, should be to_temp_green < to_temp_yellow < TO_TEMP_RED\n");
	}
	return count;
}

struct kobject *chrdev_kobj;
struct kobj_attribute to_temp_green_attr = __ATTR(to_temp_green, 0664, sysfs_green_show, sysfs_green_store);
struct kobj_attribute to_temp_yellow_attr = __ATTR(to_temp_yellow, 0664, sysfs_yellow_show, sysfs_yellow_store); // chrdev_value
struct kobj_attribute to_temp_red_attr = __ATTR(to_temp_red, 0664, sysfs_red_show, sysfs_red_store);
/*************** END Sysfs functions **********************/

// Module Initialization function
static int __init temp_led_init(void)
{
	is_open = 0;
	data_size = 0;

	// Allocate device number
	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME); // dev = Major, 0 = Minor, 1 = device, DEVICE_NAME
	if (major < 0)
	{
		pr_err("chrdev: register_chrdev failed %d\n", major);
		return major;
	}
	pr_info("-------------chrdev: register_chrdev ok, major = %d minor = %d-------------\n", MAJOR(dev), MINOR(dev));
	
	// Creating cdev structure
	cdev_init(&chrdev_cdev, &fops);

	// Adding character device to the system
	if ((cdev_add(&chrdev_cdev, dev, 1)) < 0)
	{ // 1 = number of devices in the system
		pr_err("chrdev: cannot add the device to the system\n");
		goto cdev_err;
	}
	pr_info("chrdev: cdev created successfully\n");

	// Create device class
	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass))
	{
		goto class_err;
	}
	pr_info("chrdev: device class created successfully\n");

	// Create device node
	pdev = device_create(pclass, NULL, dev, NULL, CLASS_NAME"0"); //    /dev/chrdev0     CLASS_NAME
	if (IS_ERR(pdev))
	{
		goto device_err;
	}
	pr_info("chrdev: device node created successfully\n");

	// Create a directory in /sys/kernel/
	chrdev_kobj = kobject_create_and_add(DEVICE_NAME, kernel_kobj); // "chrdev_sysfs"

	// Creating sysfs file for to_temp_green
	if (sysfs_create_file(chrdev_kobj, &to_temp_green_attr.attr))
	{
		pr_err("chrdev: cannot create sysfs file to_temp_green_attr\n");
		goto sysfs_err;
	}
	// Creating sysfs file for to_temp_yellow
	if (sysfs_create_file(chrdev_kobj, &to_temp_yellow_attr.attr))
	{
		pr_err("chrdev: cannot create sysfs file to_temp_yellow_attr\n");
		goto sysfs_err;
	}
	// Creating sysfs file for to_temp_red
	if (sysfs_create_file(chrdev_kobj, &to_temp_red_attr.attr))
	{
		pr_err("chrdev: cannot create sysfs file to_temp_red_attr\n");
		goto sysfs_err;
	}
	/* procfs */
	// Create proc directory
	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_folder)
	{
		pr_err("Failed to create /proc/%s\n", PROC_DIR_NAME);
		return -ENOMEM;
	}

	// Create proc file
	proc_file = proc_create(PROC_FILE_NAME, 0444, proc_folder, &read_temp_fops);
	if (!proc_file)
	{
		pr_err("Failed to create /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		proc_remove(proc_folder);
		return -ENOMEM;
	}
	/* end procfs */

	// LED Driver Registration
	if (gpio_request_array(led_array, ARRAY_SIZE(led_array)))
	{
		pr_err("Failed to request GPIO array\n");
		return -1;
	}

	// Timer Init
	timer_setup(&timer_blink, timer_blink_callback, 0);
	timer_setup(&timer_thermal, thermal_callback, 0);

	// Get Thermal Zone
	tz = thermal_zone_get_zone_by_name("cpu-thermal");
	if (!tz)
	{
		pr_err("thermal_zone_get_zone_by_name error\n");
		return -EINVAL;
	}
	if (thermal_zone_get_temp(tz, &temp))
	{
		pr_err("Failed to get temperature\n");
	}
	else
	{
		pr_info("CPU Temperature: %d\n", temp / 1000);
	}

	// Start Timer
	mod_timer(&timer_blink, jiffies + msecs_to_jiffies(TIMEOUT));
	mod_timer(&timer_thermal, jiffies + msecs_to_jiffies(TIMEOUT));

	// pr_info("Hello Module Inserted /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
	pr_info("-------------chrdev: device driver insmod successfully!-------------\n");
	// pr_info("-------------chrdev: device node created successfully!-------------\n");
	return 0;

sysfs_err:
	kobject_put(chrdev_kobj);
	sysfs_remove_file(kernel_kobj, &to_temp_green_attr.attr);
	sysfs_remove_file(kernel_kobj, &to_temp_yellow_attr.attr);
	sysfs_remove_file(kernel_kobj, &to_temp_red_attr.attr);
device_err:
	unregister_chrdev_region(dev, 1);
class_err:
	class_destroy(pclass);
cdev_err:
	cdev_del(&chrdev_cdev);
	
	return 0;
}

static void __exit temp_led_exit(void)
{
	kobject_put(chrdev_kobj);
	sysfs_remove_file(kernel_kobj, &to_temp_green_attr.attr);
	sysfs_remove_file(kernel_kobj, &to_temp_yellow_attr.attr);
	sysfs_remove_file(kernel_kobj, &to_temp_red_attr.attr);

	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(dev, 1); // 1 = number of devices in the system
	// Remove Timer
	del_timer(&timer_blink);
	del_timer(&timer_thermal);
	// LED Driver De-registration
	gpio_free_array(led_array, ARRAY_SIZE(led_array));
	// Remove Proc File
	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("chrdev: Module Removed /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
}

module_init(temp_led_init);
module_exit(temp_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vyshnevskiy Mykhailo");
MODULE_DESCRIPTION("A driver for measuring CPU temperature using the (cpu-thermal) sensor and indicating temperature using three LEDs: a green LED blinks when the temperature is below 40 degrees, a yellow LED blinks when the temperature is below 60 degrees, and a red LED blinks when the temperature is below 75 degrees. All LEDs remain on when the temperature is above 75 degrees, with the red LED blinking. Additionally, the driver responds to the command (cat /proc/hello/dummy) and outputs the message (temperature = %d.%d Grad)");
MODULE_VERSION("0.2");