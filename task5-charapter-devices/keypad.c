#include <linux/string.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include "keypad.h"

MODULE_LICENSE("GPL");

const uint8_t rows[] = {ROW1, ROW2, ROW3, ROW4};
const uint8_t cols[] = {COL1, COL2, COL3, COL4};
const uint8_t key[NUM_COL_ROW][NUM_COL_ROW] = {
	{'1', '2', '3', '+'},
	{'4', '5', '6', '-'},
	{'7', '8', '9', '*'},
	{'<', '0', '=', '/'}
	};
uint8_t clearBuffer[MAX_CLEAN_BUFFER] = {0};
uint8_t last = 0;

enum calc_out_e calc_out = ALL;
struct calc_s calc = {0, 0, 0xFFFFFFFF, 0};

struct calc_s *get_calc()
{
	return &calc;
}

enum calc_out_e get_calc_out()
{
	return calc_out;
}

void put_calc_out(enum calc_out_e out)
{
	calc_out = out;
}

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

void pr_calc(uint32_t pr_param)
{
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

int matrix_check_key(void)
{
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