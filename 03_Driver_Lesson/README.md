

Task:
- Writing Value to Driver 
- Reading Value from Driver
- BUFFER_SIZE 1024
- array char data_buffer[BUFFER_SIZE]
- is used to copy data into struct file_operations fops
- console program **main_user_prog.c** to check the task
 ioctl:
1. Writing Value to Driver
##
sudo insmod /home/pi/prog1/temp_led_plus_module.ko 

![](https://github.com/MihailV1/test/blob/Mykhailo_Vyshnevskyi/Screenshot_1.png)

2. Reading Value from Driver
##
![](https://github.com/MihailV1/test/blob/Mykhailo_Vyshnevskyi/Screenshot_2.png)

3. CLEAR_BUFFER buffer clearing: size=0, '\0'
##
![](https://github.com/MihailV1/test/blob/Mykhailo_Vyshnevskyi/Screenshot_3.png)

root@raspberrypi:/home/pi# dmesg

![](https://github.com/MihailV1/test/blob/Mykhailo_Vyshnevskyi/Screenshot_3_1.png)

4. GET_BUFFER_SIZE get the buffer size of the string + '\0' in the data_buffer array. 
##
![](https://github.com/MihailV1/test/blob/Mykhailo_Vyshnevskyi/Screenshot_4_1.png)

5. SET_BUFFER_SIZE check if val < BUFFER_SIZE 
check the contents of the array data_buffer[val], if val < BUFFER_SIZE 
![](https://github.com/MihailV1/test/blob/Mykhailo_Vyshnevskyi/Screenshot_5.png)
![](https://github.com/MihailV1/test/blob/Mykhailo_Vyshnevskyi/Screenshot_5_1.png)

# temp_led_plus_module
This driver measures CPU temperature using the "cpu-thermal" sensor and indicates temperature using three LEDs.

## Details
- File: `temp_led_plus_module.ko`

- Author: Mykhailo Vyshnevskyi
- Date: 24 March 2023
- Version: 0.2.0
- Tested with Linux raspberrypi [5.10.103+]

## Work Algorithm
                When starting the driver, all LEDs are on for 5 seconds.
                The temperature measurement interval is 5 seconds.
                A GREEN LED blinks when the temperature is below 40 degrees,
                a YELLOW LED blinks when the temperature is below 60 degrees, and
                a RED LED blinks when the temperature is below 75 degrees.
                All LEDs remain on, and the RED LED blinking when the temperature is above 75 degrees.
                Additionally, the driver (proc/chrdev_proc) responds to the command
                         "cat /proc/chrdev_proc/chrdev_temp_blink_proc"
                and outputs the message:
                           "temperature = %d.%d Grad"
 							"Led Green < %d Grad"
 							"Led Yellow < %d Grad"
 							"Led Red < %d Grad"
 							"Leds All > %d Grad"
		variables to define temperature limits in 3 ranges:
 			to_temp_green = 40000; // Green will be lit up until this temperature
 			to_temp_yellow = 60000; // Yellow will be lit up until this temperature.
 			to_temp_red = 75000; // Red will be lit up until this temperature
 		for read and write variables,
 		example:
 			/sys/kernel/chrdev_temp_blink$ sudo su // write only under super user (permission=0664)
 			/sys/kernel/chrdev_temp_blink# echo 31000 > to_temp_green
 			/sys/kernel/chrdev_temp_blink$ cat to_temp_green
                    31000
 			input condition: to_temp_green < to_temp_yellow < to_temp_red
		working with the buffer in /dev/chrdev0
 		example:
 			/dev$ sudo su // write only under super user
 			/dev# echo test messeg 123 > chrdev0
 			/dev$ cat chrdev0
 			test messeg 123
 			/dev$dmesg  //output the device number(Major,Minor) and the size of the transmitted information in bytes
	
## Usage
1. Run the driver:
`sudo insmod ./temp_led_plus_module.ko`
2. View processor temperature:
`cat /proc/chrdev_proc/chrdev_temp_blink_proc`
3. Shut down the driver:
`sudo rmmod ./temp_led_plus_module.ko`


## GPIO
- GPIO_5 - RED
- GPIO_6 - YELLOW
- GPIO_26 - GREEN

## LEDs connection diagram
<image src="https://github.com/MihailV1/gl_kernel_BaseCamp_2022_II/blob/02_Driver_Lesson/02_Driver_Lesson/block_diagram_small.jpg">


## Sources:
- Raspberry Pi Documentation
`<link>` : <https://www.raspberrypi.com/documentation/computers/linux_kernel.html#kernel>
- Linux 5.10.103
`<link>` :	<https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/snapshot/linux-915a747ac7f368a9979638e86fb9097d6a903672.tar.gz>
- Linux Kernel GlobalLogic BaseCamp lectures

- Tutorial on procfs in Linux
`<link>` :	<https://embetronicx.com/tutorials/linux/device-drivers/procfs-in-linux/>

