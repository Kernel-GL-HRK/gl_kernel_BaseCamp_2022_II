#include <linux/string.h>
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
MODULE_VERSION("1.2");

#define MAX_BUFFER 1024
#define MAX_CLEAN_BUFFER 32
//Dmesg defines
#define PR_START_CALC		0
#define PR_LOC_SEPARATOR	1
#define PR_GL_SEPARATOR		2
#define PR_CLEAR_OPERATOR	3
#define PR_CLEAR_NUM		4
#define PR_CLEAR_EMPTY		5
#define PR_ERR_ZERO			6
#define PR_ERR_OPERATOR 	7
#define PR_OPERANDS			8
#define PR_RESULT			9

// Proc defines
#define PROC_DIR_NAME "Keypad_Matrix"
#define PROC_FILE_NAME "Driver_Read"
#define PROC_BUFFER "Matrix keys:\n"\
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
					"\"*\" - (<-) clean a symbol\n"\
					"\"#\" - (=) equal and finish calculation\n"
// Defines of charatper device
#define CLASS_NAME "Keypad"
#define DEVICE_NAME "Matrix"
#define FIRST_NOTICE 	"----------------------------------------------------------------------\n"\
						"Calculator status messages will be displayed in - ?\n"\
						"To change calculator output direction, write in /dev/Matrix file:\n"\
						"\t1 - dmesg (output in syslog file);\n"\
						"\t2 - /dev/Matrix (output in device file);\n"\
						"\t3 - dmesg and /dev/Matrix.\n"\
						"----------------------------------------------------------------------\n"
#define C1 "dmesg"
#define C2 "/dev/Matrix"
#define C3 "dmesg and /dev/Matrix"
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
uint8_t buffer_chrdev[MAX_BUFFER] = {0};
dev_t MM = 0;
enum calc_out_e{
	DMESG = 1,
	DEV = 2,
	ALL = 3
}calc_out = DEV;
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
}calc = {0, 0, 0xFFFFFFFF, 0};
uint8_t clearBuffer[MAX_CLEAN_BUFFER], last = 0;
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

void pr_calc(uint32_t pr_param){
	if((calc_out == DMESG) || (calc_out == ALL))
		switch(pr_param){
			case PR_START_CALC:
				pr_notice("|matrix-calculator: CALCULATOR IS STARTED\n");
				break;
			case PR_LOC_SEPARATOR:
				pr_info("------------------------------------------------------------\n");
				break;
			case PR_GL_SEPARATOR:
				pr_info("------------------------matrix------------------------------\n");
				break;
			case PR_CLEAR_OPERATOR:
				pr_info("|matrix-clear: operator %c is delete\n", clearBuffer[last]);
				break;
			case PR_CLEAR_NUM:
				pr_info("|matrix-calculator-clear: numeric %c is delete\n", clearBuffer[last]);
				break;
			case PR_CLEAR_EMPTY:
				pr_info("|matrix-calculator-clear: buffer is empty\n");
				break;
			case PR_ERR_OPERATOR:
				pr_err("|matrix-calculator-error: operator(+,-,*,/) did not enter!\n");
				break;
			case PR_ERR_ZERO:
				pr_err("|matrix-calculator-error: can not divide by zero\n");
				break;
			case PR_OPERANDS:
				pr_info("|matrix-calculator: I{%d} %c II{%d}\n", calc.number_01, (calc.operator == '\0') ? '?' : calc.operator, calc.number_02);
				break;
			case PR_RESULT:
					pr_info("|matrix-calculator: %d %c %d = %d\n", calc.number_01, calc.operator, calc.number_02, calc.result);
				break;
	}
}

int matrix_check_key(void) {
	uint8_t row_num = 0, col_num = 0;
	uint32_t temp = 0;

	clearBuffer[last] = 0xFF;
	for(last = 1; clearBuffer < MAX_CLEAN_BUFFER; last++)
		clearBuffer[last] = 0;
	last = 0;

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
						pr_calc(PR_LOC_SEPARATOR);
						if ((clearBuffer[last] >= '0') && (clearBuffer[last] <= '9')){
							temp /= 10;
							pr_calc(PR_CLEAR_NUM);
							last--;
						}
						else if(clearBuffer[last] == 0xFF)
							pr_calc(PR_CLEAR_EMPTY);
						else {
							calc.operator = 0;
							temp = calc.number_01;
							pr_calc(PR_CLEAR_OPERATOR);
							last--;
						}
						pr_calc(PR_LOC_SEPARATOR);
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
										pr_calc(PR_ERR_ZERO);
										return 0;
									}
									else
										calc.result = calc.number_01 / calc.number_02;
									break;
								default: 
									pr_calc(PR_ERR_OPERATOR);
									return 0;
									break;
							}
									
							pr_calc(PR_RESULT);
							return 0;
						}
					if(calc.operator == '\0')
						calc.number_01 = temp;
					else
						calc.number_02 = temp;
						
					pr_calc(PR_OPERANDS);
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

static void output_choice_message(void)
{
	uint16_t i = 0, j = 0, alt = 0;
	while(FIRST_NOTICE[j] != '\0'){
		if(FIRST_NOTICE[j] == '?'){
			switch(calc_out){
				case DMESG:
					for(alt = 0; C1[alt] != '\0'; alt++)
						buffer_chrdev[i++] = C1[alt];
					break;
				case DEV:
					for(alt = 0; C2[alt] != '\0'; alt++)
						buffer_chrdev[i++] = C2[alt];
					break;
				case ALL:
					for(alt = 0; C3[alt] != '\0'; alt++)
						buffer_chrdev[i++] = C3[alt];
					break;
			}
			j++;
		}
		else
			buffer_chrdev[i++] = FIRST_NOTICE[j++];
	}
	while(i < sizeof(buffer_chrdev))
		buffer_chrdev[i++] = '\0';
}

// Charapter file operations
static int dev_open(struct inode *inodep, struct file *filep)
{ 
	pr_info("matrix-chrdev: file /dev/%s is opened\n", DEVICE_NAME);
	return 0; 
}
static int dev_close(struct inode *inodep, struct file *filep)
{
	pr_info("matrix-chrdev: file /dev/%s is closed\n", DEVICE_NAME);
	return 0;
}
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	ssize_t to_copy = sizeof(buffer_chrdev);
	uint16_t end = 0;

	if((*offset == 0)){
		output_choice_message();
		*offset = *offset + 1;
	}
	else if(calc_out == DEV || calc_out == ALL){
		if((calc.operator == '\0') && (calc.result == 0xFFFFFFFF))
			to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} ? II{%d} = ??          ", calc.number_01, calc.number_02);
		else if(calc.result == 0xFFFFFFFF){
		 	switch(calc.operator){
				case '+':
					to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} %c II{%d} = %-10d", calc.number_01, calc.operator, calc.number_02, calc.number_01 + calc.number_02);
					break;
				case '-':
					to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} %c II{%d} = %-10d", calc.number_01, calc.operator, calc.number_02, calc.number_01 - calc.number_02);
					break;
				case '*':
					to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} %c II{%d} = %-10d", calc.number_01, calc.operator, calc.number_02, calc.number_01 * calc.number_02);
					break;
				case '/':
					if(calc.number_02 == 0)
						to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} %c II{%d} = infinity", calc.number_01, calc.operator, calc.number_02);
					else
						to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} %c II{%d} = %-10d", calc.number_01, calc.operator, calc.number_02, calc.number_01 / calc.number_02);
					break;
			}
		}
		else{
			to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\n!!!Calculating is finished!!!\n");
			*offset = *offset + 1;
		}
	}

	if(*offset < 3){
			to_copy = min(to_copy, len);
			if(copy_to_user(buffer, buffer_chrdev, to_copy) != 0){
				pr_err("matrix-chrdev: dont have space to output data into file");
				return -ENOSPC;
			}
			if(*offset == 2)
				*offset = *offset + 1;
			return to_copy;
	}
	return 0;
}
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	uint8_t wr_buffer[MAX_BUFFER] = {0};
	uint32_t i;
	if(copy_from_user(wr_buffer, buffer, len)){
		pr_err("matrix-chrdev-error: can not read data from user");
		return -ENOSPC;
	}
	pr_info("matrix-chrdev: data is received");
	for(i = 0; i < MAX_BUFFER; i++){
		switch(wr_buffer[i]){
			case '1':
				calc_out = DMESG;
				pr_info("matrix-chrdev: calculator output - %s", C1);
				return len;
			case '2':
				calc_out = DEV;
				pr_info("matrix-chrdev: calculator output - %s", C2);
				return len;
			case '3':
				calc_out = ALL;
				pr_info("matrix-chrdev: calculator output - %s", C3);
				return len;
		}
	}
	pr_err("matrix-chrdev: entered wrong choice number, calculator output - %s", C3);
	calc_out = ALL;
	return len;
}

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
static ssize_t procFS_read(struct file *filep, char __user *buffer, size_t count, loff_t *offset)
{
    ssize_t to_copy = 0;
    to_copy = min(count, sizeof(PROC_BUFFER));
	if(*offset != sizeof(PROC_BUFFER)){
    	if(copy_to_user(buffer, PROC_BUFFER, to_copy) != 0)
			return -ENOSPC;
		else{ 
			*offset = to_copy;
			return *offset;
		}
	}
	else
		return 0;
}

static struct proc_ops proc_fops = {
		.proc_read = procFS_read,
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
	pr_calc(PR_GL_SEPARATOR);
	pr_calc(PR_START_CALC);
	matrix_check_key();
	pr_calc(PR_GL_SEPARATOR);
	output_choice_message();

	return 0;
// ERROR OPERATIONS
proc_err:
	proc_remove(proc_dir);
device_err:
	device_destroy(class_tree, MM);
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