# RGB leds blinking device driver
Simple example of Linux Kernel driver module demonstrating kernel space controls of GPIO, kernel timers, devsys_fs, proc_fs and kernel device tree overlays.

## Requirements
Connected RGB leds to your Raspberry via ***GPIO_17***, ***GPIO_22*** and ***GPIO_27*** pins (GPIO'S can be also customized via DT overlay) 
## Building the Module
```sh
make [KDIR=[Path to your linux kernel]]
```
The module is compiled to `gpio_leds_rgb.ko`

## Inserting into your kernel (at your own risk!)
Install with parameters:
```sh
sudo insmod gpio_leds_rgb.ko
```
Uninstall with parameters:
```sh
sudo rmmod gpio_leds_rgb.ko
```
## Gpio customization
By default leds should be connected to GPIO_17, GPIO_22 and GPIO_27 pins, but you can connect to you desired (free) GPIO pins via dt overlay in **leds-rgb.dts** :
```sh
   gpio-leds {
               	compatible = "gpio-leds,project1";
				gpio_red   = <17>;
				gpio_direction_red = "out";
				gpio_green = <27>;
				gpio_direction_green = "out";
				gpio_blue  = <22>;
				gpio_direction_blue = "out";	
        			};
			     };
```
overlay **leds-rgb.dtbo** should be placed to *boot/overlays* folder in your device and be included in boot/config.txt by adding line:
```sh
dtoverlay=leds-rgb
```
## Demo
Video demo [link](https://drive.google.com/file/d/1lp3GwCbnp7JEiYX4DR9hOiySi-j4OxM5/view?usp=share_link link) 

