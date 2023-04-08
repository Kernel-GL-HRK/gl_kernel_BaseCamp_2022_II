#include <linux/string.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>

//GPIO STATES
#define ON 1
#define OFF 0
//GPIO PINS FOR KEYPAD
#define ROW1 26
#define ROW2 19
#define ROW3 13
#define ROW4 11
#define COL1 12
#define COL2 16
#define COL3 20
#define COL4 21
//NUMBER COLLUMS OR ROWS
#define NUM_COL_ROW 4
//MAX SIZE OF TEMP BUFFER FOR CLEAN BUTTOM
#define MAX_CLEAN_BUFFER 32
//TYPE PRINT OUT
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

extern const uint8_t rows[NUM_COL_ROW];
extern const uint8_t cols[NUM_COL_ROW];
extern const uint8_t key[NUM_COL_ROW][NUM_COL_ROW];
extern uint8_t clearBuffer[MAX_CLEAN_BUFFER];
extern uint8_t last;

//THE VALUE THAT IS NEEDED FOR THE OUTPUT DIRECTION
enum calc_out_e{
	DMESG = 1,
	DEV = 2,
	ALL = 3
};
//FOR STORING VALUES OF CALCULATOR
struct calc_s {
	int32_t number_01;
	int32_t number_02;
	int32_t result;
	uint8_t operator;
};
//TO PASS THE STRUCT TO OTHER LIBRALIES
struct calc_s* get_calc(void);
//TO PASS AND RECEIVE THE ENUM TO OTHER LIBRALIES
enum calc_out_e get_calc_out(void);
void put_calc_out(enum calc_out_e out);
//SET UP GPIO OF KEYPAD
int matrix_key_setup(void);
//PRINT STATE OF CALCULATING
void pr_calc(uint32_t pr_param);
//CALCULATING
int matrix_check_key(void);