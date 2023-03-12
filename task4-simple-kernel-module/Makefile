# obj-m is predefined kbuild variable. used for modules.
ifneq ($(KERNELRELEASE),)
obj-m := main.o
else

KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -Wall -C $(KDIR)  M=$(shell pwd) modules

clean:
	$(MAKE) -C $(KDIR)  M=$(shell pwd) clean
endif
