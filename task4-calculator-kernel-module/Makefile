obj-m := calculator_module.o

KDIR ?= ../../kernel_old/linux/
ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-

default:
	$(MAKE) -C $(KDIR) M=$$PWD CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) modules

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean


.PHONY: clean default
