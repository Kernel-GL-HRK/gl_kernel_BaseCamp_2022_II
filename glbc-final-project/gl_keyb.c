// SPDX-License-Identifier: GPL-2.0
/*
 * This is a simple mpu6050 gyroscope kernel device driver.
 */

#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/time.h>
#include <linux/jiffies.h>

#define TIMEOUT		(200)		/* milliseconds */

/* LED - used for test only!!! */
#define	LED		(6)

/* column gpio OUT */
#define COL0		(5)
#define COL1		(22)
#define COL2		(27)
#define COL3		(17)

/* row gpio IN */
#define ROW0		(16)
#define ROW1		(25)
#define ROW2		(24)
#define ROW3		(23)

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

	pr_info("keyb: module loaded\n");
	return res;

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
	pr_err("keyb: GPIO initialization failed\n");
	return res;
}

static void __exit gl_keyb_exit(void)
{
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

