ifneq ($(KERNELRELEASE),)		# kbuild part of makefile

obj-m := device_bme280.o

device_bme280-objs := bme280.o chrdev.o   

# obj-m := chrdev.o
else							# normal makefile

KDIR ?= /lib/modules/`uname -r`/build
default:
	$(MAKE) -C $(KDIR) M=$$PWD
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif