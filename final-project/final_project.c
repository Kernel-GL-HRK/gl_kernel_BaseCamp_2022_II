#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>
#include "controlling_servo.h"
#include "scaning_ultrasonic.h"
#include "out_on_display.h"

int main(void)
{
	struct servo_description servo;
	struct ultrasonic_description ultrasonic;

	{//ULTRASONIC INIT
		if (open_dev_file_ultrasonic() < 0)
			return -1;

		ultrasonic = check_description_ultrasonic();
		if (ultrasonic.error < 0) {
			close_dev_file_ultrasonic();
			return -1;
		}
	}
	{//SERVO INIT
		if (open_dev_file_servo() < 0) {
			close_dev_file_ultrasonic();
			return -1;
		}

		servo = check_description_servo();
		if (servo.error < 0) {
			close_dev_file_servo();
			close_dev_file_ultrasonic();
			return -1;
		}
	}
	{//INIT SCREEN
		initscr();
		init_color_screen();
	}
	{//START WORKING
		menu();
	}
	{//END
		endwin();
		close_dev_file_servo();
		close_dev_file_ultrasonic();
	}
	return 0;
}