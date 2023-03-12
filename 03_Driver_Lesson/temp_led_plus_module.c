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
 *               Additionally, the driver (hello/dummy) responds to the command
 *                        \"cat /proc/hello/dummy\"
 *               and outputs the message
 *                        \"temperature = %d.%d Grad\"
 *  GPIO:
 *               GPIO_5     RED
 *               GPIO_6     YELLOW
 *               GPIO_26    GREEN
 *               Tested with Linux raspberrypi [5.10.103+]
 */
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>			// device fs
#include <linux/thermal.h>      // Thermal Zone  Temp
#include <linux/proc_fs.h>		// ??? чи потрібно ????
#include <linux/cdev.h>				// character device driver
#include <linux/kdev_t.h>
//#include<linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/gpio.h>         //GPIO
#include <linux/err.h>


#define CLASS_NAME "chrdev"
#define DEVICE_NAME "chrdev_temp_blink"
#define BUFFER_SIZE 1024

static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev;
dev_t dev = 0;				// Major Minor

static int major;
static int is_open;

static int data_size;
static unsigned char data_buffer[BUFFER_SIZE];

/* procfs */
#define PROC_BUFFER_SIZE 1024
#define PROC_DIR_NAME "chrdev_proc"
#define PROC_FILE_NAME "chrdev_temp_blink_proc"

// Buffer and size for the proc file
static char     procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t   procfs_buffer_size;
/* end procfs */

// Thermal zone device for temperature readings
struct thermal_zone_device *tz;
static int temp;

// GPIO pins for the LEDs
#define GPIO_5 5            // RED
#define GPIO_6 6            // YELLOW
#define GPIO_26 26          // GREEN

struct gpio led_array[] = {
	{ GPIO_5, GPIOF_OUT_INIT_HIGH, "LED_5" },
	{ GPIO_6, GPIOF_OUT_INIT_HIGH, "LED_6" },
	{ GPIO_26, GPIOF_OUT_INIT_HIGH, "LED_26" }
};
static int gpio_index;  // 0, 1, 2

// Timer Variables
#define TIMEOUT 5000   // milliseconds   (5s)
static struct timer_list timer_blink, timer_thermal;
static bool  flag_timer;

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

// Timer callback function for blinking LEDs
void timer_blink_callback(struct timer_list *data)
{
	if (flag_timer == false) {
		gpio_set_value(led_array[gpio_index].gpio, 0);
		mod_timer(&timer_blink, jiffies + msecs_to_jiffies(10));
		//pr_info("led_array[gpio_index= %d].gpio = %d\n", gpio_index, led_array[gpio_index].gpio);
	} else {
		gpio_set_value(led_array[gpio_index].gpio, 1);
		mod_timer(&timer_blink, jiffies + msecs_to_jiffies(1));
	}
	flag_timer = !flag_timer;
}

// Callback function for thermal events
void thermal_callback(struct timer_list *data)
{
	if (thermal_zone_get_temp(tz, &temp)) {
		pr_err("%s Failed to get temperature\n", __func__);
	}
	//pr_info("Temperature: %d\n", temp);
	if (temp < 40000) {                      // LED GREEN blinklicht
		gpio_index = 2;
		gpio_set_value(GPIO_5, 0);
		gpio_set_value(GPIO_6, 0);
		gpio_set_value(GPIO_26, 1);
	} else if (temp < 60000) {                // LED YELLOW blinklicht
		gpio_index = 1;
		gpio_set_value(GPIO_5, 0);
		gpio_set_value(GPIO_6, 1);
		gpio_set_value(GPIO_26, 0);
	} else if (temp < 75000) {                // LED RED blinklicht
		gpio_index = 0;
		gpio_set_value(GPIO_5, 1);
		gpio_set_value(GPIO_6, 0);
		gpio_set_value(GPIO_26, 0);
	} else {                                  // LED RED blinklicht, YELLOW und GREEN licht
		gpio_index = 0;
		gpio_set_value(GPIO_5, 1);
		gpio_set_value(GPIO_6, 1);
		gpio_set_value(GPIO_26, 1);
	}
	mod_timer(&timer_thermal, jiffies + msecs_to_jiffies(TIMEOUT));
}

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open){
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

	pr_info("chrdev: read from file %s\n", filep->f_path.dentry->d_name.name);	// .dentry->d_iname
	pr_info("chrdev: read from device %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));

	if (len > data_size) len = data_size;

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
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

	pr_info("chrdev: write to file %s\n", filep->f_path.dentry->d_name.name); //filep->f_inode.dentry->d_iname
	pr_info("chrdev: write to device %d:%d\n", imajor(filep->f_inode), iminor(filep->f_inode));

	data_size = len;
	if (data_size > BUFFER_SIZE) data_size = BUFFER_SIZE;

	ret = copy_from_user(data_buffer, buffer, data_size);
	if (ret) {
		pr_err("chrdev: copy_from_user failed: %d\n", ret);
		return -EFAULT;		
	}

	pr_info("chrdev: %d bytes written\n", data_size);
	return data_size;
}

/* procfs */
static ssize_t hello_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
	// Get the temperature from the thermal zone
	if (thermal_zone_get_temp(tz, &temp)) {
		pr_err("Failed to get temperature\n");
	}

	// Determine the size of the buffer to be read
	procfs_buffer_size = min(count, (size_t)PROC_BUFFER_SIZE);

	// If the current position is greater than or equal to the buffer size, there's nothing more to read
	if (*pos >= procfs_buffer_size) {
		return 0;
	}

	// Format the temperature string to be written to the buffer
	sprintf(procfs_buffer, "CPU temperature = %d.%d Grad\n", temp/1000, temp%1000);

	// If reading the buffer would cause a buffer overflow, adjust the size of the buffer accordingly
	if (*pos + procfs_buffer_size > PROC_BUFFER_SIZE) {
		procfs_buffer_size = PROC_BUFFER_SIZE - *pos;
	}

	// Copy the buffer contents to user space
	if (copy_to_user(buf, procfs_buffer + *pos, procfs_buffer_size)) {
		return -EFAULT;
	}

	// Update the current position in the buffer
	*pos += procfs_buffer_size;

	// Return the number of bytes read
	return procfs_buffer_size;
}

// Create procfs file for temperature
static const struct proc_ops hello_fops = {
	.proc_read = hello_read,
};
/* end procfs */

static struct file_operations fops =
{
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};


// Module Initialization function
static int __init temp_led_init(void)
{
	is_open = 0;
	data_size = 0;

	// Allocate device number
	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);			// dev = Major, 0 = Minor, 1 = device, DEVICE_NAME
	if (major < 0){
		pr_err("chrdev: register_chrdev failed %d\n", major);
		return major;
	}
	pr_info("-------------chrdev: register_chrdev ok, major = %d minor = %d-------------\n", MAJOR(dev), MINOR(dev));

	cdev_init(&chrdev_cdev, &fops);
	if ((cdev_add(&chrdev_cdev,dev,1)) < 0){						// 1 = number of devices in the system
		pr_err("chrdev: cannot add the device to the system\n");
		goto cdev_err;
	}
	pr_info("chrdev: cdev created successfully\n");

	// Create device class
	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)){
		goto class_err;
	}
	pr_info("chrdev: device class created successfully\n");

	// Create device node
	pdev = device_create(pclass, NULL, dev, NULL, DEVICE_NAME"0");		//    /dev/chrdev_temp_blink0     CLASS_NAME
	if (IS_ERR(pdev)){
		goto device_err;
	}
	pr_info("chrdev: device node created successfully\n");

	/* procfs */
	// Create proc directory
	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_folder) {
		pr_err("Failed to create /proc/%s\n", PROC_DIR_NAME);
		return -ENOMEM;
	}

	// Create proc file
	proc_file = proc_create(PROC_FILE_NAME, 0444, proc_folder, &hello_fops);
	if (!proc_file) {
		pr_err("Failed to create /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		proc_remove(proc_folder);
		return -ENOMEM;
	}
	/* end procfs */

	//LED Driver Registration
	if (gpio_request_array(led_array, ARRAY_SIZE(led_array))) {
		pr_err("Failed to request GPIO array\n");
		return -1;
	}

	//Timer Init
	timer_setup(&timer_blink, timer_blink_callback, 0);
	timer_setup(&timer_thermal, thermal_callback, 0);

	// Get Thermal Zone
	tz = thermal_zone_get_zone_by_name("cpu-thermal");
	if (!tz) {
		pr_err("thermal_zone_get_zone_by_name error\n");
		return -EINVAL;
	}
	if (thermal_zone_get_temp(tz, &temp)) {
		pr_err("Failed to get temperature\n");
	} else {
		pr_info("CPU Temperature: %d\n", temp / 1000);
	}

	// Start Timer
	mod_timer(&timer_blink, jiffies + msecs_to_jiffies(TIMEOUT));
	mod_timer(&timer_thermal, jiffies + msecs_to_jiffies(TIMEOUT));

	// pr_info("Hello Module Inserted /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
	pr_info("-------------chrdev: device node created successfully!-------------\n");
	return 0;

	device_err:
		class_destroy(pclass);
	class_err:
		cdev_del(&chrdev_cdev);
	cdev_err:
		unregister_chrdev_region(dev, 1);
	return 0;
}

static void __exit temp_led_exit(void)
{
	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(dev, 1);   // 1 = number of devices in the system
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