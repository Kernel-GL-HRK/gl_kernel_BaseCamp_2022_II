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
    int temperature = 0;
// volatile sig_atomic_t received_signal = 0;

int encoder_fd, display_fd, temp_fd = 0;

void my_func()
{
    char buffer[256];
    char enc_buffer[256];
    ssize_t bytesRead, bytesWritten;

    bytesRead = read(temp_fd, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        printf("Failed to read from temp device");
        return;
    }    

    bytesRead = read(encoder_fd, enc_buffer, sizeof(enc_buffer));
    if (bytesRead < 0) {
        printf("Failed to read from temp device");
        return;
    }

    encoder_value = atoi(enc_buffer);
    temperature = atoi(buffer);
    temperature /= 100;

    char display_buffer[256];
    snprintf(display_buffer, sizeof(display_buffer), "GetTemp\n%d C  \nSetTemp\n%d C        ", temperature, encoder_value);

    bytesWritten = write(display_fd, display_buffer, strlen(display_buffer));
    if (bytesWritten < 0) {
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

    int socket_desc;
    struct sockaddr_in server;
    char *message = "ON";

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

     printf("Connected to ESP8266");

    while (1) {
        my_func();


        if(encoder_value > temperature){
            if (send(socket_desc, message, strlen(message), 0) < 0) {
                printf("Send failed");
                return 1;
            }

            printf("Command sent to ESP8266");
        }

    }

    close(encoder_fd);
    close(display_fd);
    close(temp_fd);

    return EXIT_SUCCESS;
}
