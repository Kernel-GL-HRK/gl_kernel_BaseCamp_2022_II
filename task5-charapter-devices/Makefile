INS=CharDev

ifneq ($(KERNELRELEASE),)
    obj-m := $(INS).o
else
    KDIR ?= /lib/modules/`uname -r`/build

.PHONY: default clean ins rm

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

ins: default
	sudo insmod "${INS}.ko" &

rm:
	sudo rmmod ${INS}

endif
