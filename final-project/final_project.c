#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>
#include "controlling_servo.h"
#include "scaning_ultrasonic.h"

int main()
{
    open_dev_file_servo();
    open_dev_file_ultrasonic();
    
    check_description_ultrasonic();
	get_distance_ultrasonic();

	check_description_servo();
    turn_servo(44);
    get_angle_servo();

    close_dev_file_servo();
    close_dev_file_ultrasonic();

    return 0;
}