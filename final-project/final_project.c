#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>
#include "controlling_servo.h"

int main()
{
    FILE *ultrasound_fd;
    open_dev_file_servo();
    ultrasound_fd = fopen("/dev/ultrasound_control", "r+");
    if (ultrasound_fd == NULL) {
        perror("Error opening ultrasound_control");
        return -1;
    }

	

	check_description_servo();
    turn_servo(53);
    get_angle_servo();
    
    close_dev_file_servo();
    fclose(ultrasound_fd);
    return 0;
}