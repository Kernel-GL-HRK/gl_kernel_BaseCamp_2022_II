ifneq ($(KERNELRELEASE),)
# Kbuild part of makefile
obj-m := my_module.o
else
# build for current kernel if KDIR is not specified
KDIR ?= /lib/modules/`uname -r`/build
default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD)
endif
