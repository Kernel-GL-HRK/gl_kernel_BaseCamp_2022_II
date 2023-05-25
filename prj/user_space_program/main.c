#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

#include "cdev_ioctl.h"
#include "color.h"

#define ENCODER_DEVICE "/dev/encoder_driver0"
#define DISPLAY_DEVICE "/dev/st77350"
#define TEMP_DEVICE "/dev/device_bme2800"

int encoder_value = 0;
float temperature = 0;
int encoder_fd, display_fd, temp_fd = 0;

void set_xy(int x, int y)
{
    ioctl(display_fd, CDEV_SET_X, (int16_t *) &x);
    ioctl(display_fd, CDEV_SET_Y, (int16_t *) &y);
}

void display_init(void)
{
    ssize_t bytes_written;
    int16_t set_font_size = 3;
	int16_t set_text_color = WHITE;
	int16_t set_bg_color = BLACK;
    int16_t curren_temp = 0;

    ioctl(display_fd, CDEV_SET_FONT_SIZE, (int16_t *) &set_font_size);
    ioctl(display_fd, CDEV_SET_TEXT_COLOR, (int16_t *) &set_text_color);
    ioctl(display_fd, CDEV_SET_BG_COLOR, (int16_t *) &set_bg_color);
    ioctl(display_fd, CDEV_CLEAR_BUFFER);

    char display_buffer[256];
    snprintf(display_buffer, sizeof(display_buffer), "GetTemp\n  \nSetTemp ");
    bytes_written = write(display_fd, display_buffer, strlen(display_buffer));
    if (bytes_written < 0) {
        printf("Failed to write to destination device");
        return;
    }

    ioctl(temp_fd, CDEV_GET_TEMP, (int32_t *) &curren_temp);
    curren_temp /= 100;
    curren_temp--;
    ioctl(encoder_fd, CDEV_SET_ENCODER, (int32_t *) &curren_temp);

    set_text_color = RED;
    ioctl(display_fd, CDEV_SET_TEXT_COLOR, (int16_t *) &set_text_color);
}

// void my_func()
// {
//     char buffer[256];
//     char enc_buffer[256];
//     ssize_t bytes_read, bytes_written;

//     bytes_read = read(temp_fd, buffer, sizeof(buffer));
//     if (bytes_read < 0) {
//         printf("Failed to read from temp device");
//         return;
//     }

//     bytes_read = read(encoder_fd, enc_buffer, sizeof(enc_buffer));
//     if (bytes_read < 0) {
//         printf("Failed to read from temp device");
//         return;
//     }

//     encoder_value = atoi(enc_buffer);
//     temperature = atoi(buffer);
//     temperature /= 100;

//     char display_buffer[256];
//     snprintf(display_buffer, sizeof(display_buffer), "GetTemp\n%d C  \nSetTemp\n%d C        ", temperature, encoder_value);

//     bytes_written = write(display_fd, display_buffer, strlen(display_buffer));
//     if (bytes_written < 0) {
//         printf("Failed to write to destination device");
//         return;
//     }
// }


void my_func()
{
    char buffer[256];
    char enc_buffer[256];
    ssize_t bytes_read, bytes_written;

    bytes_read = read(temp_fd, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        printf("Failed to read from temp device");
        return;
    }

    bytes_read = read(encoder_fd, enc_buffer, sizeof(enc_buffer));
    if (bytes_read < 0) {
        printf("Failed to read from temp device");
        return;
    }

    encoder_value = atoi(enc_buffer);
    temperature = atoi(buffer);
    temperature /= 100;

    char display_buffer[256];

    set_xy(1, 4);
    snprintf(display_buffer, sizeof(display_buffer), "%.1f C ", temperature);
    bytes_written = write(display_fd, display_buffer, strlen(display_buffer));
    if (bytes_written < 0) {
        printf("Failed to write to destination device");
        return;
    }

    set_xy(1, 10);
    snprintf(display_buffer, sizeof(display_buffer), "%d C  ",encoder_value);
    bytes_written = write(display_fd, display_buffer, strlen(display_buffer));
    if (bytes_written < 0) {
        printf("Failed to write to destination device");
        return;
    }

}

int main(int argc, char *argv[])
{
    encoder_fd = open(ENCODER_DEVICE, O_RDONLY);
    if (encoder_fd < 0) {
        printf("Failed to open source device");
        return EXIT_FAILURE;
    }

    display_fd = open(DISPLAY_DEVICE, O_WRONLY);
    if (display_fd < 0) {
        printf("Failed to open destination device");
        close(encoder_fd);
        return EXIT_FAILURE;
    }

    temp_fd = open(TEMP_DEVICE, O_RDONLY);
    if (temp_fd < 0) {
        printf("Failed to open temp device");
        close(encoder_fd);
        close(display_fd);
        return EXIT_FAILURE;
    }

    display_init();

    int socket_desc;
    struct sockaddr_in server;
    char *message_on = "ON";
    char *message_off = "OFF";
    int command_sent = 0;

    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Could not create socket");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("192.168.0.75");
    server.sin_port = htons(PORT);

    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connect error");
        return 1;
    }

    printf("Connected to ESP8266\n");

    while (1) {
        my_func();

        if (encoder_value > temperature && !command_sent) {
            if (send(socket_desc, message_on, strlen(message_on), 0) < 0) {
                printf("Send failed");
                return 1;
            }

            printf("Command sent to ESP8266: ON\n");
            command_sent = 1;
        } else if (encoder_value <= temperature && command_sent) {
            if (send(socket_desc, message_off, strlen(message_off), 0) < 0) {
                printf("Send failed");
                return 1;
            }

            printf("Command sent to ESP8266: OFF\n");
            command_sent = 0;
        }
        else
            sleep(1);
    }

    close(encoder_fd);
    close(display_fd);
    close(temp_fd);

    return EXIT_SUCCESS;
}
