ifneq ($(KERNELRELEASE),)
obj-m := calc_module.o
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