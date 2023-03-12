obj-m += temp_led_plus_module.o

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

KDIR=/home/user/BaseCamp2023A/WildExperiments_rpi-5.10.103/linux
#KDIR ?= "/lib/modules/$(shell uname -r)/build"


all:
	make -C $(KDIR) M=$(shell pwd) modules

clean:
	rm -f ${TARGET} *.o *.a *.so *.gch *.xml *.mod *.symvers *.order *.mod.c

clean_all:
	make -C $(KDIR) M=$(shell pwd) clean
	
