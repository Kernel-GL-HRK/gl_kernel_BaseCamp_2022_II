# temp_led_module

This driver measures CPU temperature using the "cpu-thermal" sensor and indicates temperature using three LEDs.

## Details
- File: `temp_led_module.ko`
- Author: Mykhailo Vyshnevskyi
- Date: 10 March 2023
- Version: 0.1.0
- Tested with Linux raspberrypi [5.10.103+]

## Work Algorithm
- When starting the driver, all LEDs are on for 5 seconds.
- The temperature measurement interval is 5 seconds.
- A GREEN LED blinks when the temperature is below 40 degrees.
- A YELLOW LED blinks when the temperature is below 60 degrees.
- A RED LED blinks when the temperature is below 75 degrees.
- All LEDs remain on, and the RED LED blinks when the temperature is above 75 degrees.
- Additionally, the driver (hello/dummy) responds to the command 
-	`cat /proc/hello/dummy` 
- and outputs the message 
-	`temperature = %d.%d Grad`

## GPIO
- GPIO_5 - RED
- GPIO_6 - YELLOW
- GPIO_26 - GREEN

## Usage
1. Run the driver:
	udo insmod ./temp_led_module.ko
2. 2. View processor temperature:
	cat /proc/hello/dummy
3. Shut down the driver:
	sudo rmmod ./temp_led_module.ko

## Makefile
Makefile for cross-compiling a kernel module

# module name 
obj-m += temp_led_module.o

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

# path for CROSS COMPILE
KDIR=/home/user/BaseCamp2023A/WildExperiments_rpi-5.10.103/linux

# to compile on the Raspberry Pi
# KDIR ?= "/lib/modules/$(shell uname -r)/build"

all:
	make -C $(KDIR) M=$(shell pwd) modules
clean:
	rm -f ${TARGET} *.o *.a *.so *.gch *.xml *.mod *.symvers *.order *.mod.c
clean_all:
	make -C $(KDIR) M=$(shell pwd) clean

## LEDs connection diagram
![My Image](https://drive.google.com/file/d/1rfrgtPdDDv1S7TKzq5YTvsGthlRzrk3B/view?usp=sharing)


## Test video


## Sources:
- Raspberry Pi Documentation
	https://www.raspberrypi.com/documentation/computers/linux_kernel.html#kernel
- Linux 5.10.103
	https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/snapshot/linux-915a747ac7f368a9979638e86fb9097d6a903672.tar.gz
- Linux Kernel GlobalLogic BaseCamp lectures

- Tutorial on procfs in Linux
	https://embetronicx.com/tutorials/linux/device-drivers/procfs-in-linux/
- ChatGPT
