#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Muravka Roman");
MODULE_DESCRIPTION("Simple charapter device - Matrix keypad");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.1");

#define MAX_BUFFER 1024
// Proc defines
#define PROC_DIR_NAME "Keypad_Matrix"
#define PROC_FILE_NAME "Driver_Read"
#define PROC_BUFFER = "Matrix keys:\n"\
						"|-----------------|\n"\
						"| [1] [2] [3] [A] |\n"\
						"| [4] [5] [6] [B] |\n"\
						"| [7] [8] [9] [C] |\n"\
						"| [*] [0] [#] [D] |\n"\
						"|-----------------|\n"\
						"Number keys(0-9) responsible for numerics.\n"\
						"\"A\" - (+) add;\n"\
						"\"B\" - (-) subtract;\n"\
						"\"C\" - (*) multiply;\n"\
						"\"D\" - (/) divide;\n"\
						"\"*\" - (<-) clean a symbol"\
						"\"#\" - (=) equal and finish calculation"
// Defines of charatper device
#define CLASS_NAME "Keypad"
#define DEVICE_NAME "Matrix"
// GPIO defines
#define ON 1
#define OFF 0
#define ROW1 26
#define ROW2 19
#define ROW3 13
#define ROW4 11
#define COL1 12
#define COL2 16
#define COL3 20
#define COL4 21
#define NUM_COL_ROW 4
// Charapter struct and variables
static struct class *class_tree;
static struct device *device_file;
static struct cdev device;
dev_t MM = 0;
// Matrix variables
const uint8_t rows[] = {ROW1, ROW2, ROW3, ROW4};
const uint8_t cols[] = {COL1, COL2, COL3, COL4};
const uint8_t key[NUM_COL_ROW][NUM_COL_ROW] = {
	{'1', '2', '3', '+'},
	{'4', '5', '6', '-'},
	{'7', '8', '9', '*'},
	{'<', '0', '=', '/'}
	};

int matrix_key_setup(void) 
{
	uint8_t i;
	for (i = 0; i < NUM_COL_ROW; i++) {
		gpio_export(rows[i], 1);
		gpio_export(cols[i], 1);

		gpio_direction_input(rows[i]);
		gpio_direction_input(cols[i]);
	}
	pr_info("matrix: gpio is configurated\n");
	return 0;
}

struct calc_s {
	int32_t number_01;
	int32_t number_02;
	int32_t result;
	uint8_t operator;
}calc = {0, 0, 0, 0};

int debounce(int gpio)
{
	int i = 0;
	while(1){
		for(i = 0; i < 200; i++){
			if (gpio_get_value(gpio) == OFF)
				return 0;
		}
	}
	return 0;
}

int matrix_check_key(void) {
	uint8_t row_num = 0, col_num = 0;
	uint8_t last = 0, clearBuffer[32] = {0};
	uint32_t temp = 0;
	clearBuffer[last] = 0xFF;
	while (1) {
		gpio_direction_output(rows[row_num], ON);

		for (col_num = 0; col_num < NUM_COL_ROW; col_num++) {
			if (gpio_get_value(cols[col_num]) == ON) {
				debounce(cols[col_num]);
				if ((key[row_num][col_num] >= '0') && (key[row_num][col_num] <= '9'))
					temp = (temp * 10) + (key[row_num][col_num] - '0');

				switch (key[row_num][col_num]) {
					case '+':
					case '-':
					case '*':
					case '/':
						if ((!calc.number_02) && (calc.operator == '\0')) {
							calc.operator = key[row_num][col_num];
							clearBuffer[++last] = key[row_num][col_num];
							temp = 0;
						}
						break;
					case '<':
						pr_info("|----------------------------------------------------------|\n");
						if ((clearBuffer[last] >= '0') && (clearBuffer[last] <= '9')){
							temp /= 10;
							pr_info("|matrix-calculator-clear: numeric %c is delete              |\n", clearBuffer[last]);
							last--;
						}
						else if(clearBuffer[last] == 0xFF)
							pr_info("matrix-calculator-clear: buffer is empty                   |\n");
						else {
							calc.operator = 0;
							temp = calc.number_01;
							pr_info("|matrix-clear: operator %c is delete                        |\n", clearBuffer[last]);
							last--;
						}
						pr_info("|----------------------------------------------------------|\n");
						break;
					case '=':
							switch(calc.operator){
								case '+':
									calc.result = calc.number_01 + calc.number_02;
									break;
								case '-':
									calc.result = calc.number_01 - calc.number_02;
									break;
								case '*':
									calc.result = calc.number_01 * calc.number_02;
									break;
								case '/':
									if(calc.number_02 == 0){
										pr_err("|matrix-calculator-error: can not divide by zero           |\n");
										pr_info("|-----------------------matrix-----------------------------|\n");
										return 0;
									}
									else
										calc.result = calc.number_01 / calc.number_02;
									break;
								default: 
									pr_err("|matrix-error: operator(+,-,*,/) did not enter!            |\n");
									pr_info("|-----------------------matrix-----------------------------|\n");
									return 0;
									break;
							}
									
							pr_info("|matrix-calculator: %2d %c %2d = %4d                       |\n", calc.number_01, calc.operator, calc.number_02, calc.result);
							pr_info("|-----------------------matrix-----------------------------|\n");
							return 0;
						}
					if(calc.operator == '\0')
						calc.number_01 = temp;
					else
						calc.number_02 = temp;
						
					pr_info("|matrix-calculator: I{%2d} %c II{%2d}                         |\n", calc.number_01, (calc.operator == '\0') ? '?' : calc.operator, calc.number_02);
					if(key[row_num][col_num] != '<')
						if((key[row_num][col_num] >= '0') && (key[row_num][col_num] <= '9'))
							clearBuffer[++last] = key[row_num][col_num];
			}
		}

		gpio_direction_input(rows[row_num]);
		if (++row_num == NUM_COL_ROW)
			row_num = 0;
	}
}

// Charapter file operations
static int dev_open(struct inode *inodep, struct file *filep) { return 0; }
static int dev_close(struct inode *inodep, struct file *filep) { return 0; }
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) { return 0; }
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) { return 0; }

static struct file_operations dev_fops = {
		.open = dev_open,
		.release = dev_close,
		.read = dev_read,
		.write = dev_write,
};

// Proc struct and variables

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_dir;

// Proc file operations
static ssize_t proc_read(struct file *filep, char __user *buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, copied;
	
	return 0; 
}

static struct proc_ops proc_fops = {
		.proc_read = proc_read,
};

static int matrix_init(void) 
{
	// CHARAPTER
	if (alloc_chrdev_region(&MM, 0, 1, DEVICE_NAME) < 0) {
		pr_err("matrix: ERROR: no free Major-number");
		return -ENOMEM;
	}
	pr_info("matrix: free identifiers: Major = %d, Minor = %d;", MAJOR(MM), MINOR(MM));

	cdev_init(&device, &dev_fops);
	if (cdev_add(&device, MM, 1) < 0) {
		pr_err("matrix: ERROR: cannot add the device to the system\n");
		goto cdev_err;
	}
	pr_info("matrix: device added to the system");

	class_tree = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(class_tree)) {
		pr_err("matrix: ERROR: An error occurred while creating a class on the path /sys/class/%s" CLASS_NAME);
		goto class_err;
	}
	pr_info("matrix: device class on the path /sys/class/%s created succesfully\n", CLASS_NAME);

	device_file = device_create(class_tree, NULL, MM, NULL, DEVICE_NAME);
	if (IS_ERR(device_file)) {
		pr_err("matrix: ERROR: An error occurred while creating a device file on the path /dev/%s", DEVICE_NAME);
		goto device_err;
	}
	pr_info("matrix: device file on the path /dev/%s created succesfully\n", DEVICE_NAME);

	// PROC
	proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_dir) {
		pr_err("matrix: ERROR: couldn`t initialize /proc/%s\n", PROC_DIR_NAME);
		return -ENOMEM;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0444, proc_dir, &proc_fops);
	if (!proc_file) {
		pr_err("matrix: ERROR: couldn`t initialize /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		goto proc_err;
	}
	pr_info("matrix: /proc/%s/%s created\n", PROC_DIR_NAME, PROC_FILE_NAME);
	matrix_key_setup();
	pr_info("|-----------------------matrix-----------------------------|\n");
	pr_notice("|matrix-calculator: calculater is started                  |\n");
	matrix_check_key();
	return 0;
// ERROR OPERATIONS
proc_err:
	proc_remove(proc_dir);
device_err:
	device_destroy(class_tree, MM);
class_destroy:
	class_destroy(class_tree);
class_err:
	cdev_del(&device);
cdev_err:
	unregister_chrdev_region(MM, 1);
	return -1;
}

static void matrix_exit(void) 
{
	device_destroy(class_tree, MM);
	class_destroy(class_tree);
	cdev_del(&device);
	unregister_chrdev_region(MM, 1);
	pr_info("matrix: device removed\n");

	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_info("matrix: procFS removed\n");
	uint8_t i;
	for (i = 0; i < NUM_COL_ROW; i++) {
		gpio_unexport(rows[i]);
		gpio_unexport(cols[i]);
	}
	pr_info("matrix: gpio is unexported\n");
	pr_info("matrix: module exited\n");

}

module_init(matrix_init);
module_exit(matrix_exit);