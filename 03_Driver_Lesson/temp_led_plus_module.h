#ifndef __TEMP_LED_PLUS_MODULE_H__
#define __TEMP_LED_PLUS_MODULE_H__

/* /sys  /dev */
#define CLASS_NAME "chrdev"
#define DEVICE_NAME "chrdev_temp_blink"
#define BUFFER_SIZE 1024

/* procfs */
#define PROC_BUFFER_SIZE 400
#define PROC_DIR_NAME "chrdev_proc"
#define PROC_FILE_NAME "chrdev_temp_blink_proc"

// // GPIO pins for the LEDs
#define GPIO_5 5   // RED
#define GPIO_6 6   // YELLOW
#define GPIO_26 26 // GREEN

// Timer Variables
#define TIMEOUT 5000 // milliseconds   (5s)

#endif /* __TEMP_LED_PLUS_MODULE_H__ */
