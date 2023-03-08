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
	`temperature = %d.%d Grad`

## GPIO
- GPIO_5 - RED
- GPIO_6 - YELLOW
- GPIO_26 - GREEN

## Usage
1. Run the driver:
`sudo insmod ./temp_led_module.ko`
2. View processor temperature:
`cat /proc/hello/dummy`
3. Shut down the driver:
`sudo rmmod ./temp_led_module.ko`


## LEDs connection diagram
(<image src="https://lh3.google.com/u/0/d/1rfrgtPdDDv1S7TKzq5YTvsGthlRzrk3B=w1920-h980-iv1">)


## Test video


## Sources:
- Raspberry Pi Documentation
`<link>` : <https://www.raspberrypi.com/documentation/computers/linux_kernel.html#kernel>
- Linux 5.10.103
`<link>` :	<https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/snapshot/linux-915a747ac7f368a9979638e86fb9097d6a903672.tar.gz>
- Linux Kernel GlobalLogic BaseCamp lectures

- Tutorial on procfs in Linux
`<link>` :	<https://embetronicx.com/tutorials/linux/device-drivers/procfs-in-linux/>
- ChatGPT

