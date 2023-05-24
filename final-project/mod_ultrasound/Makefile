MODULE_NAME = ultrasound_mod
MAIN_OBJ = ultrasound_main.o
DEVICE_TREE = dt_ultrasound
LIBRARY_OBJS = sys_dev.o proc.o platform_driver.o timer_and_irq.o

ifneq ($(KERNELRELEASE),)
        obj-m := $(MODULE_NAME).o
        $(MODULE_NAME)-y := $(LIBRARY_OBJS) $(MAIN_OBJ)
else
        KDIR ?= /lib/modules/`uname -r`/build

.PHONY: mod clean

mod:
        $(MAKE) -C $(KDIR) M=$(PWD)

clean:
        $(MAKE) -C $(KDIR) M=$(PWD) clean

mod-i: mod
        sudo insmod $(MODULE_NAME).ko

mod-r:
        sudo rmmod $(MODULE_NAME)

dt:
        dtc -@ -I dts -O dtb -o $(DEVICE_TREE).dtbo $(DEVICE_TREE).dts

dt-i: dt
        sudo dtoverlay $(DEVICE_TREE).dtbo

dt-r:
        sudo dtoverlay -r $(DEVICE_TREE)
endif