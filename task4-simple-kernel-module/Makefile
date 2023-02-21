obj-m := hellau_module.o

export ARCH = arm
export CROSS_COMPILE = arm-linux-gnueabihf-
export KDIR = /home/user/pi/linux

#KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$$PWD modules

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
