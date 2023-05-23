#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>

int main() {
    int servo_fd, ultrasound_fd;

    servo_fd = open("/dev/servo_control", O_RDWR);
    if (servo_fd < 0) {
        perror("Error opening servo_control");
        return -1;
    }

    ultrasound_fd = open("/dev/ultrasound_control", O_RDWR);
    if (ultrasound_fd < 0) {
        perror("Error opening ultrasound_control");
        close(servo_fd);
        return -1;
    }

    close(ultrasound_fd);
    close(servo_fd);

    return 0;
}