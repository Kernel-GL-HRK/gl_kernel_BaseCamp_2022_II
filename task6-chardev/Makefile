ifneq ($(KERNELRELEASE),)
obj-m := chardev_module.o
else

KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

endif

.PHONY: checkpatch
checkpatch:
	./checkpatch.pl --no-tree -f *.c
	./checkpatch.pl --no-tree -f *.h