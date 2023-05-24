ifneq ($(KERNELRELEASE),)
# Kbuild part of makefile
obj-m := gl_mpu6050.o
CFLAGS_gl_mpu6050.o := -DDEBUG
else

export ARCH = arm
export CROSS_COMPILE = arm-linux-gnueabihf-
export KDIR = ~/gl_base_camp/raspberrypi/linux/

PWD := $(CURDIR)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f *.dtbo

dtb:
	dtc -I dts -O dtb ./gl_mpu6050-overlay.dts -o ./gl_mpu6050.dtbo

endif

