MODULE_U = CharDev_mod
MAIN_FILE = CharDev.o
LIBS = proc_ops.o keypad.o dev_ops.o sys_ops.o

ifneq ($(KERNELRELEASE),)
    obj-m := $(MODULE_U).o
	$(MODULE_U)-y = $(LIBS) $(MAIN_FILE)
else
    KDIR ?= /lib/modules/`uname -r`/build

.PHONY: default clean ins rm

default:
	$(MAKE) -C $(KDIR) M=$(PWD)

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

ins: default
	sudo insmod "$(MODULE_U).ko" &

rm:
	sudo rmmod $(MODULE_U)

endif
