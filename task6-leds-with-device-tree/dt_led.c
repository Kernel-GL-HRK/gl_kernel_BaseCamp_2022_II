#include <linux/init.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/kdev_t.h>
#include <linux/proc_fs.h>
#include <linux/err.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include "ioctl-cmd.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muravka Roman");
MODULE_DESCRIPTION("RGB with using device tree and timers");
//GPIO STATUS
#define ON	1
#define OFF	0
//PROC
#define PROC_FILE_NAME	"rgb-leds"
#define PROC_INFO_PART_1	"GPIO LEDS:\n"		\
							"RED	- [%2d]\n"	\
							"GREEN	- [%2d]\n"	\
							"BLUE	- [%2d]\n"
#define PROC_INFO_PART_2	"[%s]\t"
#define MAX_BUFFER			1024
#define MAX_HEADER_ROWS		20
#define MAX_HEADER_COLS		2
const uint8_t gpio_to_pin[] = {//HEADER 
	0xff, 0xff,//			[---*---][---*---]
	2, 0xff,//			[gpio_02][---*---]
	3, 0xff,//			[gpio_03][---*---]
	4, 14,//			[gpio_04][gpio_14]
	0xff, 15,//			[---*---][gpio_15]			
	17, 18,//			[gpio_17][gpio_18]
	27, 0xff,//			[gpio_27][---*---]
	22, 23,//			[gpio_22][gpio_23]
	0xff, 24,//			[---*---][gpio_24]
	10, 0xff,//			[gpio_10][---*---]
	9, 25,//			[gpio_09][gpio_25]
	11, 8,//			[gpio_11][gpio_08]
	0xff, 7,//			[---*---][gpio_07]
	0, 1,//				[gpio_00][gpio_01]
	5, 0xff,//			[gpio_05][---*---]
	6, 12,//			[gpio_06][gpio_12]
	13, 0xff,//			[gpio_13][---*---]
	19, 16,//			[gpio_19][gpio_16]
	26, 20,//			[gpio_26][gpio_20]
	0xff, 21//			[---*---][gpio_21]
};
enum {
	INFO_PART_1,
	INFO_PART_2,
	END
};
//CHECK GPIO NUMBERS
#define RED_NUM_IS_GPIO		0b100
#define GREEN_NUM_IS_GPIO	0b010
#define BLUE_NUM_IS_GPIO	0b001								
//DEV and CLASS
#define CLASS_NAME	"rgb"
#define DEVICE_NAME	"setup_leds"
//DEVICE TREE PROPERTY NAMES
#define DT_RED_LED		"red-led"
#define DT_GREEN_LED	"green-led"
#define DT_BLUE_LED		"blue-led"

struct proc_dir_entry *proc_file;//proc/*

struct class *class_folder;	//sys/class/*
struct cdev device;//in system
struct device *dev_file;//dev/*

dev_t MM;//major and minor

struct timer_list timer_red;//timer for red led
uint32_t msec_red = 1000;			
struct timer_list timer_green;//timer for green led
uint32_t msec_green = 1000;
struct timer_list timer_blue;//timer for blue led
uint32_t msec_blue = 1000;

struct rgb_s{//for storing values of device tree 
	uint32_t red;
	uint32_t green;
	uint32_t blue;
}rgb = {0, 0, 0};

struct of_device_id id[] = {//for struct of platform driver
	{.compatible = "brightlight,mydev,RGB",},
	{},
};
MODULE_DEVICE_TABLE(of, id);//optional. It must be used, if funchion dt_probe() is in separate lib.

/*It is need to check values of gpio leds in device tree file*/
int32_t check_gpio(uint32_t red, uint32_t green, uint32_t blue) 
{
	uint32_t i, j;
	uint32_t curPin = 0;
	int32_t result = 0;
	
	if ((red == green) || (red == blue) || (green == blue)) 
		return -1;

	for(i = 1; i <= MAX_HEADER_ROWS; i++) {
		for(j = 1; j <= MAX_HEADER_COLS; j++, curPin++) {
				
			if (gpio_to_pin[curPin] == red)
				result |= RED_NUM_IS_GPIO;
			else if (gpio_to_pin[curPin] == green)
				result |= GREEN_NUM_IS_GPIO;
			else if (gpio_to_pin[curPin] == blue)
				result |= BLUE_NUM_IS_GPIO;
		}
	}
		
	return result;
}

/*Togle red led in interrupt*/
void interrupt_red(struct timer_list *data)
{
	switch (gpio_get_value(rgb.red)) {
	case OFF:
		gpio_set_value(rgb.red, ON);
		break;
	case ON:
		gpio_set_value(rgb.red, OFF);
	break;
	}

	if(msec_red == OFF)
		gpio_set_value(rgb.red, OFF);
	else
		mod_timer(&timer_red, jiffies + msecs_to_jiffies(msec_red));
}
/*Togle green led in interrupt*/
void interrupt_green(struct timer_list *data)
{
	switch (gpio_get_value(rgb.green)) {
	case OFF:
		gpio_set_value(rgb.green, ON);
		break;
	case ON:
		gpio_set_value(rgb.green, OFF);
	break;
	}

	if(msec_green == OFF)
		gpio_set_value(rgb.green, OFF);
	else
		mod_timer(&timer_green, jiffies + msecs_to_jiffies(msec_green));
}
/*Togle blue led in interrupt*/
void interrupt_blue(struct timer_list *data)
{
	switch (gpio_get_value(rgb.blue)) {
	case OFF:
		gpio_set_value(rgb.blue, ON);
		break;
	case ON:
		gpio_set_value(rgb.blue, OFF);
	break;
	}

	if(msec_blue == OFF)
		gpio_set_value(rgb.blue, OFF);
	else
		mod_timer(&timer_blue, jiffies + msecs_to_jiffies(msec_blue));
}
/*This function will be called if device tree for this module would be downloaded*/
int dt_probe(struct platform_device *pdev)
{
	struct device *dev_node = &pdev->dev; //there is all properties of device tree
	
	if (device_property_present(dev_node, DT_RED_LED)) {
		int err;
		err = device_property_read_u32(dev_node, DT_RED_LED, &rgb.red);
		if (err) {
			pr_err("rgb-gpio: cannot read %s property\n", DT_RED_LED);
			return -ENOKEY;
		}
	}
	pr_info("rgb-gpio: value of %s property: %d\n", DT_RED_LED, rgb.red);

	if (device_property_present(dev_node, DT_GREEN_LED)) {
		int err;
		err = device_property_read_u32(dev_node, DT_GREEN_LED, &rgb.green);
		if (err) {
			pr_err("rgb-gpio: cannot read %s property\n", DT_GREEN_LED);
			return -ENOKEY;
		}
	}
	pr_info("rgb-gpio: value of %s property: %d\n", DT_GREEN_LED, rgb.green);

	if (device_property_present(dev_node, DT_BLUE_LED)) {
		int err;
		err = device_property_read_u32(dev_node, DT_BLUE_LED, &rgb.blue);
		if (err) {
			pr_err("rgb-gpio: cannot read %s property\n", DT_BLUE_LED);
			return -ENOKEY;
		}
	}
	pr_info("rgb-gpio: value of %s property: %d\n", DT_BLUE_LED, rgb.blue);
	
	if (check_gpio(rgb.red, rgb.green, rgb.blue) < 0) {
		pr_err("rgb-check: gpio pins must have diference values\n");
		return 0;
	}
	if (check_gpio(rgb.red, rgb.green, rgb.blue) & RED_NUM_IS_GPIO) {
		gpio_request(rgb.red, DT_RED_LED);
		gpio_direction_output(rgb.red, OFF);
		timer_setup(&timer_red, interrupt_red, 0);
		mod_timer(&timer_red, jiffies + msecs_to_jiffies(msec_red));
	} else {
		pr_warn("rgb-check: red led is not connected to gpio: [%d]\n", rgb.red);
	}
	if (check_gpio(rgb.red, rgb.green, rgb.blue) & GREEN_NUM_IS_GPIO) {
		gpio_request(rgb.green, DT_GREEN_LED);
		gpio_direction_output(rgb.green, OFF);
		timer_setup(&timer_green, interrupt_green, 0);
		mod_timer(&timer_green, jiffies + msecs_to_jiffies(msec_green));
	} else {
		pr_warn("rgb-check: green led is not connected to gpio: [%d]\n", rgb.green);
	}
	if (check_gpio(rgb.red, rgb.green, rgb.blue) & BLUE_NUM_IS_GPIO) {
		gpio_request(rgb.blue, DT_BLUE_LED);
		gpio_direction_output(rgb.blue, OFF);
		timer_setup(&timer_blue, interrupt_blue, 0);
		mod_timer(&timer_blue, jiffies + msecs_to_jiffies(msec_blue));
	} else {
		pr_warn("rgb-check: blue led is not connected to gpio: [%d]\n", rgb.blue);
	}
	if (check_gpio(rgb.red, rgb.green, rgb.blue) == 0)
		pr_err("rgb-check: not one led is connected to gpio\n");
	return 0;	
}
/*This function will be called if device tree for this module would be uploaded*/
int dt_remove(struct platform_device *pdev)
{
	if (check_gpio(rgb.red, rgb.green, rgb.blue) & RED_NUM_IS_GPIO) {
		gpio_set_value(rgb.red, OFF);
		gpio_free(rgb.red);
		del_timer(&timer_red);
	}
	if (check_gpio(rgb.red, rgb.green, rgb.blue) & GREEN_NUM_IS_GPIO) {
		gpio_set_value(rgb.green, OFF);
		gpio_free(rgb.green);
		del_timer(&timer_green);
	}
	if (check_gpio(rgb.red, rgb.green, rgb.blue) & BLUE_NUM_IS_GPIO) {
		gpio_set_value(rgb.blue, OFF);
		gpio_free(rgb.blue);
		del_timer(&timer_blue);
	}
	pr_info("rgb-rm: device tree was deleted\n");
	return 0;
}


struct platform_driver rgb_driver = { //for registration platform driver and working funcs probe and remove
	.probe = dt_probe,
	.remove = dt_remove,
	.driver = {
		.name = "RGB_leds",
		.of_match_table = id, //when this .compitible will be found in system, dt_probe will be called
	},
};
/*It display information about values that store in device tree into procFS*/
ssize_t procfs_read(struct file *filep, char *buf, size_t len, loff_t *of) {
	uint8_t proc_info[MAX_BUFFER] = {0};
	uint8_t temp_buf[256] = {0};
	uint32_t i, j, curPin = 0;
	size_t to_copy;

	
	
	switch (*of) {
	case INFO_PART_1:
		if (check_gpio(rgb.red, rgb.green, rgb.blue) < 0) {
			to_copy = sprintf(proc_info, PROC_INFO_PART_1"\n!!!LEDS DON`T WORK, BECAUSE LEDS HAVE EQUALLY NUMBER!!!\n", rgb.red, rgb.green, rgb.blue);
			*of = END;
		} else if ((check_gpio(rgb.red, rgb.green, rgb.blue) == 0)) {
			to_copy = sprintf(proc_info, PROC_INFO_PART_1"\n!!!LEDS DON`T WORK, BECAUSE NOT ONE LED IS CONNECTED TO GPIO!!!\n", rgb.red, rgb.green, rgb.blue);
			*of = END;
		} else {
			*of = *of + 1;
			to_copy = sprintf(proc_info, PROC_INFO_PART_1, rgb.red, rgb.green, rgb.blue);
		}
		goto copy_to_buffer;
		break;
	case INFO_PART_2:
		for(i = 1; i <= MAX_HEADER_ROWS; i++) {
			for(j = 1; j <= MAX_HEADER_COLS; j++, curPin++) {
				if (gpio_to_pin[curPin] == rgb.red) {
					if((j % 2) != 0) 
						sprintf(temp_buf, PROC_INFO_PART_2, " red ");
					else
						sprintf(temp_buf, PROC_INFO_PART_2"\n", " red ");
				} else if (gpio_to_pin[curPin] == rgb.green) {
					if((j % 2) != 0) 
						sprintf(temp_buf, PROC_INFO_PART_2, "green");
					else
						sprintf(temp_buf, PROC_INFO_PART_2"\n", "green");
				} else if (gpio_to_pin[curPin] == rgb.blue) {
					if((j % 2) != 0) 
						sprintf(temp_buf, PROC_INFO_PART_2, "blue ");
					else
						sprintf(temp_buf, PROC_INFO_PART_2"\n", "blue ");
				} else {
					if((j % 2) != 0) 
						sprintf(temp_buf, PROC_INFO_PART_2, "  *  ");
					else
						sprintf(temp_buf, PROC_INFO_PART_2"\n", "  *  ");
				}
				strcat(proc_info, temp_buf);
			}
		}
		*of = *of + 1;
		to_copy = MAX_BUFFER;
		goto copy_to_buffer;
		break;
	case END:
	default:
		return 0;

copy_to_buffer:
	to_copy = min(to_copy, len);
	if (copy_to_user(buf, proc_info, to_copy)) {
		pr_err("rgb-proc: data is not copied to buffer");
		return -ENODATA;
	}
	return to_copy;
}
}

struct proc_ops proc_fops = {
	.proc_read = procfs_read,
};

long ioctl_op(struct file *filep, unsigned int cmd, unsigned long arg)  {
	uint32_t msec_rgb;
	size_t to_copy;

	if (_IOC_TYPE(cmd) != CH_DEV) {
		pr_err("rgb-magic: device was chosen wrong");
		return -ENOKEY;
	}

	if (_IOC_DIR(cmd) == _IOC_WRITE) {
		if (copy_from_user(&msec_rgb, (uint32_t *)arg, sizeof(arg))) {
			pr_err("rgb-ioctl: can not copy time work for leds\n");
		}
		switch (_IOC_NR(cmd)) {
		case RED:
			msec_red = msec_rgb;
			if(msec_red == OFF)
				pr_info("rgb-ioctl: red led is switched off");
			else
				pr_info("rgb-ioctl: red led will work every %d msec\n", msec_red);
			mod_timer(&timer_red, jiffies + msecs_to_jiffies(msec_red));
			break;
		case GREEN:
			msec_green = msec_rgb;
			if(msec_green == OFF)
				pr_info("rgb-ioctl: green led is switched off");
			else
				pr_info("rgb-ioctl: green led will work every %d msec\n", msec_green);
			mod_timer(&timer_green, jiffies + msecs_to_jiffies(msec_green));
			break;
		case BLUE:
			msec_blue = msec_rgb;
			if(msec_blue == OFF)
				pr_info("rgb-ioctl: blue led is switched off");
			else
				pr_info("rgb-ioctl: blue led will work every %d msec\n", msec_blue);
			mod_timer(&timer_blue, jiffies + msecs_to_jiffies(msec_blue));
			break;
		case LEDS_OFF:
			msec_red = OFF;
			msec_green = OFF;
			msec_blue = OFF;
			pr_info("rgb-ioctl: all leds is switched off\n");
			break;
		default:
			pr_warn("rgb-ioctl: your command is wrong\n");
		}
	}
	return 0;
}

struct file_operations dev_fops = {
	.unlocked_ioctl = ioctl_op,
};

int rgb_init(void)
{
	pr_info("-----------------------------------------------------------\n");
	pr_info("rgb: module was downloaded\n");
	{//Creating proc file
		proc_file = proc_create(PROC_FILE_NAME, 0666, NULL, &proc_fops);
		if (proc_file == NULL) {
			pr_err("rgb-proc: cannot create a file /proc/%s", PROC_FILE_NAME);
			return -ENOMEM;
		}
		pr_info("rgb-proc: file /proc/%s, was created", PROC_FILE_NAME);

	}
	{//Reserving Major and Minor numbers
		int32_t err;
		err = alloc_chrdev_region(&MM, 0, 1, "rgb-leds");
		if (err < 0) {
			pr_err("rgb-MM: major and minor cannot create\n");
			return err;
		}
		pr_info("rgb-MM: major and minor are created\n");
	}
	{//Initializing and adding device to system
		cdev_init(&device, &dev_fops);
		if (cdev_add(&device, MM, 1)) {
			pr_err("rgb-cdev: cannot add device to system\n");
			goto cdev_err;
		}
		pr_info("rgb-cdev: device was added to system\n");
	}
	{//Creating class folder and dev file
		class_folder = class_create(THIS_MODULE, CLASS_NAME);
		if (IS_ERR(class_folder)) {
			pr_err("rgb-class: cannot create class on path /sys/class/%s\n", CLASS_NAME);
			goto class_err;
		}
		pr_info ("rgb-class: class on path /sys/class/%s, was created\n", CLASS_NAME);
		
		dev_file = device_create(class_folder, NULL, MM, NULL, DEVICE_NAME);
		if (IS_ERR(dev_file)) {
			pr_err("rgb-dev: file on path /dev/%s, cannot create\n", DEVICE_NAME);
			goto device_err;
		}
		pr_info("rgb-dev: file on path /dev/%s, was created\n", DEVICE_NAME);
	}
	{//Setting up gpio by device tree
		if (platform_driver_register(&rgb_driver)) {
			pr_err("rgb-dt: cannot create driver\n");
			goto device_tree_err;
		}
	}
	pr_info("rgb-dt: rgb_driver was register\n");

	return 0;

device_tree_err:
	device_destroy(class_folder, MM);
device_err:
	class_destroy(class_folder);
class_err:
	cdev_del(&device);
cdev_err:
	unregister_chrdev_region(MM, 1);
	proc_remove(proc_file);
	return -ENOMEM;
}

void rgb_exit(void)
{
	device_destroy(class_folder, MM);
	class_destroy(class_folder);
	cdev_del(&device);
	unregister_chrdev_region(MM, 1);
	proc_remove(proc_file);
	platform_driver_unregister(&rgb_driver);

	pr_info("rgb: module was uploaded\n");
	pr_info("-----------------------------------------------------------\n");

}

module_init(rgb_init);
module_exit(rgb_exit);