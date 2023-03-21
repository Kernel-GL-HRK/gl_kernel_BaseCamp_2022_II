//#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "chrdev_ioctl.h"


int main()
{
    int fd;
    //int32_t value, number;

    int choice;

    int flag = 1;

    printf("\nOpening Driver\n");
    fd = open(CDEV_FILE, O_RDWR);
    if(fd < 0){
        printf("Cannot open device file...\n");
        return 0;
    }

    data_buffer_info_t q;

    while (flag)
    {
        printf("Choose an option 1: Writing Value to Driver\n");
        printf("Choose an option 2: Reading Value from Driver\n");
        printf("Choose an option 3: CLEAR_BUFFER\n");
        printf("Choose an option 4: GET_BUFFER_SIZE\n");
        printf("Choose an option 5: SET_BUFFER_SIZE\n");
        printf("Choose an option 0: EXIT\n\n");
        printf("Enter number :");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("Enter the Value to Driver: ");
            scanf("%d", &q.ioctl_from_user_value);
            printf("Writing Value to Driver\n\n");
            ioctl(fd, CDEV_WR_VALUE, &q);

            break;
        case 2:
            ioctl(fd, CDEV_RD_VALUE, &q);
            printf("Reading Value from Driver: %d\n\n", q.ioctl_to_user_value);

            break;
        case 3:
            printf("CDEV_CLEAR_BUFFER array data_buffer\n");
            ioctl(fd, CDEV_CLEAR_BUFFER);
            getchar();
            break;
        case 4:
            printf("GET_BUFFER_SIZE from Driver /dev/chrdev0\n");
            ioctl(fd, CDEV_GET_BUFFER_SIZE, &q);
            printf("data_buffer[BUFFER_SIZE] array size is %d Bytes\n", q.ioctl_arr_size_val);
            printf("the size of the string including the character '0' in the data_buffer array is %d Bytes\n\n", q.ioctl_str_size_val);
            break;
        case 5:
            printf("SET_BUFFER_SIZE.\n");
            printf("get an array with a maximum length of 1023 characters\n");
            printf("Enter the Value to Driver: ");
            scanf("%d", &q.ioctl_from_user_value);
            if (q.ioctl_from_user_value <= 1023)
            {
                ioctl(fd, CDEV_WRRD_ARR_VALUE, &q);
            }else{
                printf("Upppp-s!\n");
                break;
            }
            q.ioctl_to_user_value = 1;
            //  Waiting for the array to be copied
            while (q.ioctl_to_user_value)
            {
                ioctl(fd, CDEV_WRRD_ARR_VALUE, &q);
            }

            ioctl(fd, CDEV_WRRD_ARR_VALUE, &q);
            printf("data_buffer[%d] START_ARR->%.*s", q.ioctl_from_user_value, q.ioctl_from_user_value, q.ioctl_data_buffer);
            printf("<-END_ARR\n\n");
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