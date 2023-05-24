obj-m := led_driver.o
 
KDIR = /home/user/pi/linux

export ARCH = arm
export CROSS_COMPILE = arm-linux-gnueabihf-
 
all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean
