obj-m := chrdev_proc_sysfs_ioctl.o

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
export KDIR=~/Embear/linux

PWD :=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
