ifneq ($(KERNELRELEASE),)		# kbuild part of makefile

obj-m := device_st7735.o

device_st7735-objs := st7735_chrdev.o st7735_spi.o  

#my_st7735fb-y += $(KDIR)/drivers/staging/fbtft/fbtft.o

else							# normal makefile

KDIR ?= /lib/modules/`uname -r`/build
default:
	$(MAKE) -C $(KDIR) M=$$PWD
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif