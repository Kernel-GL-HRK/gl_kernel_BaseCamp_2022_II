ifneq ($(KERNELRELEASE),)
obj-m := chardev.o
else

KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

endif

check:
	../checkpatch.pl --no-tree -f *.c *.h

insmod:
	sudo insmod chardev.ko

rmmod:
	sudo rmmod chardev
