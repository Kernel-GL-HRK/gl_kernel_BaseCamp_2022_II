#include <curses.h>
#include <stdio.h>
#include <stdint.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "out_on_display.h"
#include "controlling_servo.h"
#include "scaning_ultrasonic.h"

static const uint8_t gpio_to_pin[] = {//HEADER
	NON, NON,//			[---*---][---*---]
	2, NON,//			[gpio_02][---*---]
	3, GND,//			[gpio_03][---*---]
	4, 14,//			[gpio_04][gpio_14]
	GND, 15,//			[---*---][gpio_15]
	17, 18,//			[gpio_17][gpio_18]
	27, GND,//			[gpio_27][---*---]
	22, 23,//			[gpio_22][gpio_23]
	NON, 24,//			[---*---][gpio_24]
	10, NON,//			[gpio_10][---*---]
	9, GND,//			[gpio_09][gpio_25]
	11, 8,//			[gpio_11][gpio_08]
	GND, 7,//			[---*---][gpio_07]
	0, 1,//				[gpio_00][gpio_01]
	5, NON,//			[gpio_05][---*---]
	6, 12,//			[gpio_06][gpio_12]
	13, GND,//			[gpio_13][---*---]
	19, 16,//			[gpio_19][gpio_16]
	26, 20,//			[gpio_26][gpio_20]
	GND, 21//			[---*---][gpio_21]
};

void init_color_screen(void)
{
	start_color(); // начало работы с цветами
	init_pair(BLUE_ON_BLACK, COLOR_BLUE, COLOR_BLACK);
	init_pair(WHITE_ON_BLUE, COLOR_WHITE, COLOR_BLUE);
	init_pair(RED_ON_YELLOW, COLOR_BLACK, COLOR_YELLOW);
}

void menu(void)
{
	uint32_t choose;
	uint32_t max_distance = 0, mode = 0;

	do {
		clear();
		printw("MENU:\n");
		printw("\t1 - SHOW HEADER RASSPBERRY PI FOR CONNECTION\n");
		printw("\t2 - SCAN THIS AREA\n");
		printw("\t3 - SHOW SERVO DESCRIPTION\n");
		printw("\t4 - SHOW ULTRASONIC DESCRIPTION\n");
		printw("\t5 - EXIT FROM PROGRAM\n");


		printw("\nYour choice: ");
		scanw("%d", &choose);

		switch (choose) {
		case OUT_HEAD:
			clear();
			output_header();
			break;
		case SCAN_ULTRASONIC:
			clear();
			printw("\nMode of working device\n");
			printw("Enter max distance: ");
			scanw("%d\n", &max_distance);
			if (max_distance > 400) {
				printw("!!!Wrong choice!!!\n");
				break;
			}
			if (max_distance < 20) {
				printw("!!!Too little distance!!!\n");
				break;
			}
			printw("Choose mode of control:\n");
			printw("1 - Automatic control\n");
			printw("2 - Manual control\n");
			printw("Your choice: ");
			scanw("%d", &mode);
			printw("\n");
			if ((mode != AUTO_CONTROL) && (mode != MANUAL_CONTROL)) {
				printw("!!!Wrong choice!!!\n");
				break;
			}
			refresh();
			output_distance(max_distance, mode);
			break;
		case SHOW_SERVO_DESC:
			show_servo_description();
			break;
		case SHOW_US_DESC:
			show_ultrasonic_description();
			break;
		case EXIT:
			printw("\nExiting from the program\n");
			break;
		default:
			printw("\n!!!Wrong choice!!!\n");
			break;
		}
		printw("\nPress any buttom to continue...\n");
		refresh();
		getch();
	} while (choose != EXIT);
}

void output_header(void)
{
	int32_t first_col = NUM_FIRST_COL, second_col = NUM_SECOND_COL;
	struct ultrasonic_description ultrasonic;
	int32_t count_cell = 0;
	int8_t to_copy[MAX_BUFFER_OUT] = {0};

	ultrasonic = check_description_ultrasonic();

	for (uint32_t i = 0; i < NUM_HEADER_ROWS; i++) {
		for (uint32_t j = 0; j < NUM_HEADER_COLS; j++, count_cell++) {
			if (SERVO_PIN_CHANNEL == gpio_to_pin[count_cell])
				sprintf(to_copy, SERVO_PIN_NAME);
			else if (ultrasonic.echo_pin == gpio_to_pin[count_cell])
				sprintf(to_copy, ULTRASONIC_ECHO_PIN);
			else if (ultrasonic.trig_pin == gpio_to_pin[count_cell])
				sprintf(to_copy, ULTRASONIC_TRIG_PIN);
			else if (GND == gpio_to_pin[count_cell])
				sprintf(to_copy, "---GND---");
			else
				sprintf(to_copy, "----*----");
			if ((j + 1) == NUM_FIRST_COL)
				printw(HEADER_FIRST_COL, first_col, to_copy);
			else
				printw(HEADER_SECOND_COL, to_copy, second_col);
		}
		first_col += NUM_HEADER_COLS;
		second_col += NUM_HEADER_COLS;
	}
}
uint32_t last_angle;

void output_distance(uint32_t max_distance, uint32_t control)
{
	uint32_t max_line_len;
	uint32_t left_right_line_len, center_line_len;
	struct servo_description servo = check_description_servo();
	uint32_t current_distance, current_angle;
	uint32_t direction = ROTATE_RIGHT;
	uint16_t exit = 0;

	{//TURN SERVO TO DEFAULT ANGLE
		clear();
		printw("Rotating servo to 0...\n");
		refresh();
		turn_servo(MIN_ANGLE);
		clear();
	}
	{//SET UP PRINTW BEFORE WORKING
		noecho();
		keypad(stdscr, TRUE);
		nodelay(stdscr, TRUE);
	}
	{//(6, 7, 8, 9) -> 5; (4, 3, 2, 1) -> 0
		while (max_distance % 5 != 0) 
		max_distance--;
	}
	max_line_len = (max_distance / 5) * 2 + MIN_LINE_LEN;

	while (!exit) {
		int8_t ch;

		current_distance = get_distance_ultrasonic();
		current_angle = get_angle_servo();

		clear();
		printw("\n");

		for (int32_t i = max_distance; i >= 0; i -= 5) {
			uint8_t left_region[MAX_BUFFER_CLEAN_IMG] = {0}, center_region[MAX_BUFFER_CLEAN_IMG] = {0}, right_region[MAX_BUFFER_CLEAN_IMG] = {0};

			center_line_len = (i / 5) * 2 + MIN_LINE_LEN;
			left_right_line_len = max_line_len - center_line_len;

			for (int32_t j = 0; j < left_right_line_len / 2; j++) {//***\/***
				left_region[j] = UNTRACKED_CELL;
				right_region[j] = UNTRACKED_CELL;
			}
			for (int32_t j = 0; j < center_line_len; j++) {//\***/
				if (((current_distance - i) <= 2) || ((i - current_distance) <= 2)) {
					if ((j + 2) == (center_line_len / 2)) {
						int8_t temp[MAX_BUFFER_CLEAN_IMG] = {0};

						sprintf(temp, "%3d", current_distance);
						for (int i = 0; temp[i] != '\0'; i++)
							if (temp[i] == ' ')
								temp[i] = '-';

						strcat(center_region, temp);

						j += 2;
					} else
						center_region[j] = BARRIER_CELL;

				} else
					center_region[j] = TRACKED_CELL;
			}
			/*OUTPUT LINE OF SCANNING*/
			printw("%3d -|- ", i);
			printw("%s", left_region);
			if (((current_distance - i) <= 2) || ((i - current_distance) <= 2)) {
				attron(COLOR_PAIR(RED_ON_YELLOW));
				printw("\\%s/", center_region);
				attroff(COLOR_PAIR(RED_ON_YELLOW));
			} else {
				attron(COLOR_PAIR(WHITE_ON_BLUE));
				printw("\\%s/", center_region);
				attroff(COLOR_PAIR(WHITE_ON_BLUE));
			}

			printw("%s\n", right_region);
		}
		{//SHOWING SENSOR IMAGE
		for (int32_t j = 0; j < left_right_line_len; j++) {
			if (j == left_right_line_len / 2)
				printw(SENS_IMG);
			else if (j == 0)
				for (int32_t i = 0; i < OFFSET_FROM_COORDINATE; i++)
					printw(" ");
			else
				printw(" ");
		}
		}
		printw("\n");
		{//SHOW SERVO AND ANIMATION OF SERVO
		for (int32_t j = 0; j < left_right_line_len; j++) {
			if (j == left_right_line_len / 2)
				if (last_angle == current_angle)
					printw(SERVO_IMG, current_angle - 1, current_angle, current_angle + 1);
				else if ((last_angle < current_angle) || (last_angle == SHIFT_SERVO_LEFT)) {
					if (last_angle != SHIFT_SERVO_LEFT) {
						printw(SERVO_IMG_SHIFT_LEFT, current_angle - 1, current_angle, current_angle + 1);
						last_angle = SHIFT_SERVO_LEFT;
					} else {
						printw(SERVO_IMG_SHIFT_RIGHT, current_angle - 1, current_angle, current_angle + 1);
						last_angle = current_angle;
					}
				} else {
					if (last_angle != SHIFT_SERVO_RIGHT) {
						printw(SERVO_IMG_SHIFT_RIGHT, current_angle - 1, current_angle, current_angle + 1);
						last_angle = SHIFT_SERVO_RIGHT;
					} else {
						printw(SERVO_IMG_SHIFT_LEFT, current_angle - 1, current_angle, current_angle + 1);
						last_angle = current_angle;
					}
				}
			else if (j == 0)
				for (int32_t i = 0; i < OFFSET_FROM_COORDINATE - 6; i++)
					printw(" ");
			else
				printw(" ");
		}
		}
		{//BORDER
			int32_t lastX, lastY;
			int32_t x = 0, y = 0;

			getyx(stdscr, lastY, lastX);
			lastY++;
			lastX++;

			for (y = 0; y <= lastY; y++) {
				mvprintw(y, 0, "|");
				mvprintw(y, lastX, "|");


				if ((y == 0) || (y == lastY)) {
					for (x = 0; x <= lastX; x++) {
						mvprintw(y, x, "-");
					}
				}
			}
		}
		{//EXPLANATION FOR MANUAL CONTROLLING
			int32_t startX, lastY;
			int32_t y = 0;

			getyx(stdscr, lastY, startX);
			if (control == MANUAL_CONTROL) {
				y += 2;
				move(y, startX);
				printw("\t\"A\" or \"<-\" = MOVE SERVO LEFT");

				y += 2;
				move(y, startX);
				printw("\t\"D\" or \"->\" = MOVE SERVO RIGHT");
			}

			y += 2;
			move(y, startX);
			printw("\t\"ESC\" = EXIT");

			lastY++;
			move(++lastY, 0);
			printw("\n\n");
		}

		refresh();
		if (control == MANUAL_CONTROL) {
			while ((ch = getch()) != EOF) {
				switch (ch) {
				case KEY_LEFT_FOR_SERVO:
				case 'a':
				case 'A':
					turn_servo(current_angle - 1);
					break;
				case KEY_RIGHT_FOR_SERVO:
				case 'd':
				case 'D':
					turn_servo(current_angle + 1);
					break;
				default:
					break;
				}
			}
		}
		if (control == AUTO_CONTROL) {
			if (current_angle == MAX_ANGLE)
				direction = ROTATE_LEFT;

			if (current_angle == MIN_ANGLE)
				direction = ROTATE_RIGHT;

			if (direction == ROTATE_LEFT)
				turn_servo(current_angle - 1);
			if (direction == ROTATE_RIGHT)
				turn_servo(current_angle + 1);
		}
		{//EXIT
			while ((ch = getch()) != EOF)
				if (ch == ESC_EXIT)
					exit = 1;
		}
		usleep(servo.speed * ONE_MSEC_IN_USEC);
	}


	keypad(stdscr, FALSE);
	nodelay(stdscr, FALSE);
	echo();
}

void show_servo_description(void)
{
	struct ultrasonic_description description;
	int32_t current_distance;

	description = check_description_ultrasonic();
	current_distance = get_distance_ultrasonic();

	clear();

	printw("Number of echo pin: [%d]\n", description.echo_pin);
	printw("Number of trig pin: [%d]\n", description.trig_pin);
	printw("Current distance: [%d]\n\n", current_distance);

	refresh();
}
void show_ultrasonic_description(void)
{
	struct servo_description description;
	int32_t current_angle;


	clear();

	description = check_description_servo();
	current_angle = get_angle_servo();

	printw("Number of pwm channel: [%d]\n", description.pwm_channel);
	printw("Speed of servo: [%d]\n", description.speed);
	printw("Mode of servo: [%s]\n", description.mode);
	printw("Current angle: [%d]\n\n", current_angle);

	refresh();
}