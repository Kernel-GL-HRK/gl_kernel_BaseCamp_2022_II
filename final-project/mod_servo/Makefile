MODULE_NAME = servo_mod
MAIN_OBJ = servo_main.o
DEVICE_TREE = dt_servo
LIBRARY_OBJS = servo_ctl.o sys_dev.o proc.o platform_driver.o

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
