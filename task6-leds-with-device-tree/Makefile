MODULE_U = dt_led_mod
MAIN_FILE = dt_led.o
DEV_TREE = devtree
LIBS =

ifneq ($(KERNELRELEASE),)
    obj-m := $(MODULE_U).o
	$(MODULE_U)-y = $(LIBS) $(MAIN_FILE)
else
    KDIR ?= /lib/modules/5.15.84+/build

.PHONY: default clean ins rm

default:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

ins: default
	sudo insmod "$(MODULE_U).ko"

rm:
	sudo rmmod $(MODULE_U)

dt:
	dtc -@ -I dts -O dtb -o $(DEV_TREE).dtbo $(DEV_TREE).dts

dt-i: dt
	sudo dtoverlay $(DEV_TREE).dtbo

dt-r:
	sudo dtoverlay -r $(DEV_TREE)
endif
