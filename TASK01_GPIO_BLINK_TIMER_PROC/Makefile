ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
obj-m := gbtp.o
else
# normal makefile
 
KDIR = /lib/modules/$(shell uname -r)/build
 
all:
	make -C $(KDIR)  M=$$PWD modules
clean:
	make -C $(KDIR)  M=$$PWD clean

endif
