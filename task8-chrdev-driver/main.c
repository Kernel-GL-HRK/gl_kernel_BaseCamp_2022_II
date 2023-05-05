#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "cdev_ioctl.h"


int main(void)
{
	int fd;
	int choice;
	int flag = 1;
	int32_t to_user_value, from_user_value, get_buffer_size, set_buffer_size;

	printf("\nOpening Driver\n");
	fd = open(CDEV_FILE, O_RDWR);
	if (fd < 0) {
		printf("Cannot open device file...\n");
		return 0;
	}

	while (flag) {
		printf("Choose an option 1: Writing Value to Driver\n");
		printf("Choose an option 2: Reading Value from Driver\n");
		printf("Choose an option 3: Clear buffer\n");
		printf("Choose an option 4: Get buffer size\n");
		printf("Choose an option 5: Set buffer size\n");
		printf("Choose an option 0: EXIT\n\n");
		printf("Enter number :");
		scanf("%d", &choice);

		switch (choice) {
		case 1:
			printf("Enter the Value to Driver: ");
			scanf("%d", &from_user_value);
			printf("Writing Value to driver\n");
			ioctl(fd, CDEV_WR_VALUE, (int32_t*) &from_user_value);
			break;
		case 2:
			ioctl(fd, CDEV_RD_VALUE, (int32_t*) &to_user_value);
			printf("Value is %d\n", to_user_value);
			break;
		case 3:
			printf("Clear buffer...\n");
			ioctl(fd, CDEV_CLEAR_BUFFER);
			//getchar();
			printf("Done\n");
			break;
		case 4:
			printf("Get buffer size from Driver /dev/chrdev0\n");
			ioctl(fd, CDEV_GET_BUFFER_SIZE, (int32_t*) &get_buffer_size);
			printf("data_buffer[BUFFER_SIZE] array size is %d Bytes\n", get_buffer_size);
			break;
		case 5:
			printf("Set buffer size\n");
			scanf("%d", &set_buffer_size);
			ioctl(fd, CDEV_SET_BUFFER_SIZE, (int32_t*) &set_buffer_size);
			break;
		case 0:
			printf("EXIT\n");
			flag = 0;
			break;
		default:
		printf("Invalid option. Please choose again.\n");
			break;
		}
	}
	printf("Closing Driver\n");
	close(fd);

	return 0;

}