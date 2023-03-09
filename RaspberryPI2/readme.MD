This is just a basic linux blinker device driver which explains about the GPIO in linux device driver.

1. Install Raspberry Pi OS (32-bit) with desktop on the SD card.
2. Then install the kernel header using 
	sudo apt install raspberrypi-kernel-headers
3. Build the driver by using Makefile
	sudo make
4. Use sudo su and enter the password if required to get the root permission
	sudo su
5. Load the driver using
	insmod driver.ko
6. Check driver
	lsmod | grep driver
7. Unload the driver using
	rmmod driver.ko
