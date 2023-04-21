This is just a basic linux device driver which explains about the GPIO in linux device driver.

1. Install Raspberry Pi OS (32-bit) with desktop on the SD card.
2. Then install the kernel header using 
	sudo apt install raspberrypi-kernel-headers
3. Build the driver by using Makefile
	sudo make
4. Use sudo su and enter the password if required to get the root permission
	sudo su
5. Load the driver using
	insmod driver.ko
6. echo 1 > /dev/raspi_gpio_device [This must turn ON the LED].
	echo 1 > /dev/raspi_gpio_device
7. echo 0 > /dev/raspi_gpio_device [This must turn OFF the LED].
	echo 0 > /dev/raspi_gpio_device
8. cat /dev/raspi_gpio_device [Read the GPIO value].
	cat /dev/raspi_gpio_device
9.Since we are exporting the GPIO 19 to the sysfs using gpio_export(), the below steps also should work.
	echo 1 > /sys/class/gpio/gpio19/value [This must turn ON the LED].
	echo 0 > /sys/class/gpio/gpio19/value [This must turn OFF the LED].
	cat /sys/class/gpio/gpio19/value [Read the GPIO value].
10. Unload the driver using
	rmmod driver.ko
