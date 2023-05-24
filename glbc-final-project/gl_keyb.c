// SPDX-License-Identifier: GPL-2.0
/*
 * This is a simple mpu6050 gyroscope kernel device driver.
 */

#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/cdev.h>

#define TIMEOUT		(100)		/* milliseconds */

/* LED - used for test only!!! */
#define	LED		(6)

/* column gpio OUT */
#define COL0		(17)
#define COL1		(27)
#define COL2		(22)
#define COL3		(5)

/* row gpio IN */
#define ROW0		(23)
#define ROW1		(24)
#define ROW2		(25)
#define ROW3		(16)

/* devfs */
#define	BUFFER_SIZE	1024
#define	CLASS_NAME	"keyb"
#define DEVICE_NAME	"gl_keyb"

static dev_t dev;
static int major;
static int is_open;
static int data_size;
static unsigned char data_buffer[BUFFER_SIZE];
static char symb_pres;
static bool keyb_prev_state;

static int dev_open(struct inode *inodep, struct file *filep);
static int dev_release(struct inode *inodep, struct file *filep);
static ssize_t dev_read(struct file *filep, char *buffer,
			size_t len, loff_t *offset);
/*
 * @0444: chardev permissions (all read).
 */
static int keyb_chardev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	add_uevent_var(env, "DEVMODE=%#o", 0444);
	return 0;
}

static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev;

static int col[] = {COL0, COL1, COL2, COL3};
static int row[] = {ROW0, ROW1, ROW2, ROW3};

static char symbol[][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

static struct gpio col_gpios[] = {
	{ COL0, GPIOF_OUT_INIT_HIGH, "col0" },
	{ COL1, GPIOF_OUT_INIT_HIGH, "col1" },
	{ COL2, GPIOF_OUT_INIT_HIGH, "col2" },
	{ COL3, GPIOF_OUT_INIT_HIGH, "col3" },
};

static struct gpio row_gpios[] = {
	{ ROW0, GPIOF_IN, "row0" },
	{ ROW1, GPIOF_IN, "row1" },
	{ ROW2, GPIOF_IN, "row2" },
	{ ROW3, GPIOF_IN, "row3" },
};

static struct timer_list etx_timer;

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.read  = dev_read,
};

void timer_callback(struct timer_list *data)
{
#define	FORWARD		'2'
#define	BACK		'8'
#define LEFT		'4'
#define RIGHT		'6'

	int i, j;
	bool keyb_current_state = false;

	/*
	 * Set each column to 0 and read the row.
	 * If the row is 0 - the button is pressed.
	 */
	for (i = 0; i < ARRAY_SIZE(col); ++i) {
		// set column to 0
		gpio_set_value(col[i], 0);
		for (j = 0; j < ARRAY_SIZE(row); ++j) {
			if (gpio_get_value(row[j]) == 0) {
				keyb_current_state = true;
				symb_pres = symbol[i][j];
				gpio_set_value(LED, 1);
				pr_debug("keyb: the pressed key is %c\n", symb_pres);
			}
		}
		// set column to 1
		gpio_set_value(col[i], 1);
	}

	if ((keyb_prev_state == 1) && (keyb_current_state == 0)) {
		// key release
		gpio_set_value(LED, 0);
		if ((symb_pres == FORWARD) || (symb_pres == BACK)
		    || (symb_pres == LEFT) || (symb_pres == RIGHT)) {
			// save direction
			sprintf(data_buffer, "%c\n", symb_pres);
			data_size = strlen(data_buffer);
		}
	}
	keyb_prev_state = keyb_current_state;

	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}

static int dev_open(struct inode *inodep, struct file *filep)
{
	if (is_open) {
		pr_err("keyb: character device already open\n");
		return -EBUSY;
	}

	is_open = 1;
	pr_debug("keyb: character device opened\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	is_open = 0;
	pr_debug("keyb: character device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer,
			size_t len, loff_t *offset)
{
	int ret;

	if (len > data_size) {
		len = data_size;
	}

	ret = copy_to_user(buffer, data_buffer, len);
	if (ret) {
		pr_err("keyb: character device copy_to_user failed: %d\n", ret);
		return -EFAULT;
	}
	data_size = 0;		/* eof for cat */

	return len;
}

/**
 * gl_keyb
 * @:
 *
 * In this module LED used for test only!
 * Initialization the gpio's column (output) and row (input)
 * for keyboard.
 */
static int __init gl_keyb_init(void)
{
	int res = 0;

	/* timer init */
	/* setup timer to call timer_callback */
	timer_setup(&etx_timer, timer_callback, 0);

	/* setup timer interval to based on TIMEOUT macro */
	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
	pr_debug("keyb: timer initialized\n");

	/* gpio */
	/* gpio LED init */
	if (gpio_is_valid(LED) == false) {
		pr_err("keyb: %d is not valid\n", LED);
		res = -EEXIST;
		goto out;
	}

	if (gpio_request(LED, "GPIO") < 0) {
		pr_err("keyb: %d request\n", LED);
		res = -ENODEV;
		goto out_led;
	}

	/* LED ON */
	gpio_direction_output(LED, 1);
	gpio_export(LED, false);
	pr_debug("keyb: LED on\n");

	/* COL0..3 - OUTPUT */
	/* COL0..3 - validation */
	if (gpio_is_valid(COL0) == false) {
		pr_err("keyb: gpio %d is not valid\n", COL0);
		res = -EEXIST;
		goto out_col;
	}

	if (gpio_is_valid(COL1) == false) {
		pr_err("keyb: gpio(%d) is not valid\n", COL1);
		res = -EEXIST;
		goto out_col;
	}

	if (gpio_is_valid(COL2) == false) {
		pr_err("keyb: gpio(%d) is not valid\n", COL2);
		res = -EEXIST;
		goto out_col;
	}

	if (gpio_is_valid(COL3) == false) {
		pr_err("keyb: gpio(%d) is not valid\n", COL3);
		res = -EEXIST;
		goto out_col;
	}

	/* COL0..3 - request */
	if (gpio_request_array(col_gpios, ARRAY_SIZE(col_gpios)) < 0) {
		pr_err("keyb: error col gpio's request\n");
		res = -ENODEV;
		goto out_col_1;
	}
	gpio_direction_output(COL0, 1);
	gpio_export(COL0, false);

	gpio_direction_output(COL1, 1);
	gpio_export(COL1, false);

	gpio_direction_output(COL2, 1);
	gpio_export(COL2, false);

	gpio_direction_output(COL3, 1);
	gpio_export(COL3, false);

	/* ROW0..3 - INPUT */
	/* ROW0..3 - validation */
	if (gpio_is_valid(ROW0) == false) {
		pr_err("keyb: gpio(%d) is not valid\n", ROW0);
		res = -EEXIST;
		goto out_row;
	}

	if (gpio_is_valid(ROW1) == false) {
		pr_err("keyb: gpio(%d) is not valid\n", ROW1);
		res = -EEXIST;
		goto out_row;
	}

	if (gpio_is_valid(ROW2) == false) {
		pr_err("keyb: gpio(%d) is not valid\n", ROW2);
		res = -EEXIST;
		goto out_row;
	}

	if (gpio_is_valid(ROW3) == false) {
		pr_err("keyb: gpio(%d) is not valid\n", ROW3);
		res = -EEXIST;
		goto out_row;
	}

	/* ROW0..3 - request */
	if (gpio_request_array(row_gpios, ARRAY_SIZE(row_gpios)) < 0) {
		pr_err("keyb: error row gpio's request\n");
		res = -ENODEV;
		goto out_row1;
	}

	gpio_direction_input(ROW0);
	gpio_export(ROW0, false);

	gpio_direction_input(ROW1);
	gpio_export(ROW1, false);

	gpio_direction_input(ROW2);
	gpio_export(ROW2, false);

	gpio_direction_input(ROW3);
	gpio_export(ROW3, false);

	/* chardev init */
	is_open = 0;
	data_size = 0;

	major = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
	if (major < 0) {
		pr_err("keyb: character device register failed %d\n", major);
		res = -EEXIST;
		goto out_major_err;
	}
	pr_debug("keyb: character device register, major = %d, minor = %d\n",
		 MAJOR(dev), MINOR(dev));

	cdev_init(&chrdev_cdev, &fops);
	if (cdev_add(&chrdev_cdev, dev, 1) < 0) {
		pr_err("keyb: cannot add the device to the system\n");
		res = -ENODEV;
		goto out_cdev_err;
	}
	pr_debug("keyb: character device created successfully\n");

	pclass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(pclass)) {
		res = -EBADF;
		goto out_class_err;
	}
	pclass->dev_uevent = keyb_chardev_uevent;
	pr_debug("keyb: character device class created successfully\n");

	pdev = device_create(pclass, NULL, dev, NULL, DEVICE_NAME);
	if (IS_ERR(pdev)) {
		res = -EBUSY;
		goto out_device_err;
	}
	pr_debug("keyb: character device /dev/%s created\n", DEVICE_NAME);

	pr_info("keyb: module loaded\n");
	return res;

out_device_err:
	class_destroy(pclass);
out_class_err:
	cdev_del(&chrdev_cdev);
out_cdev_err:
	unregister_chrdev_region(dev, 1);
out_major_err:
	gpio_unexport(ROW3);
	gpio_free(ROW3);
	gpio_unexport(ROW2);
	gpio_free(ROW2);
	gpio_unexport(ROW1);
	gpio_free(ROW1);
	gpio_unexport(ROW0);
	gpio_free(ROW0);
out_row1:
	gpio_free_array(row_gpios, ARRAY_SIZE(row_gpios));
out_row:
	gpio_set_value(COL0, 0);
	gpio_unexport(COL0);
	gpio_set_value(COL1, 0);
	gpio_unexport(COL1);
	gpio_set_value(COL2, 0);
	gpio_unexport(COL2);
	gpio_set_value(COL3, 0);
	gpio_unexport(COL3);
out_col_1:
	gpio_free_array(col_gpios, ARRAY_SIZE(col_gpios));
out_col:
	gpio_set_value(LED, 0);
	gpio_unexport(LED);
out_led:
	gpio_free(LED);
out:
	del_timer(&etx_timer);
	pr_err("keyb: GPIO initialization failed\n");
	return res;
}

static void __exit gl_keyb_exit(void)
{
	/* character device */
	device_destroy(pclass, dev);
	class_destroy(pclass);
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(dev, 1);
	pr_debug("keyb: character device /dev/%s removed\n", DEVICE_NAME);

	/* timer */
	del_timer(&etx_timer);
	pr_debug("keyb: timer de-initialized\n");

	/* ROW0..3 */
	gpio_unexport(ROW3);
	gpio_free(ROW3);
	gpio_unexport(ROW2);
	gpio_free(ROW2);
	gpio_unexport(ROW1);
	gpio_free(ROW1);
	gpio_unexport(ROW0);
	gpio_free(ROW0);
	gpio_free_array(row_gpios, ARRAY_SIZE(row_gpios));

	/* COL0..3 */
	gpio_set_value(COL3, 0);
	gpio_unexport(COL3);
	gpio_set_value(COL2, 0);
	gpio_unexport(COL2);
	gpio_set_value(COL1, 0);
	gpio_unexport(COL1);
	gpio_set_value(COL0, 0);
	gpio_unexport(COL0);
	gpio_free_array(col_gpios, ARRAY_SIZE(col_gpios));

	/* LED OFF */
	gpio_set_value(LED, 0);
	gpio_unexport(LED);
	gpio_free(LED);

	pr_info("keyb: module exited\n");
}

module_init(gl_keyb_init);
module_exit(gl_keyb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergiy Us <sergiy.us@gmail.com>");
MODULE_DESCRIPTION("A simple 4x4 matrix keyboard kernel driver");
MODULE_VERSION("0.1");

