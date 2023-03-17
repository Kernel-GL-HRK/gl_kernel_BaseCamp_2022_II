ifneq ($(KERNELRELEASE),)
	obj-m := hello.o
else 
	KDIR ?= /lib/modules/`uname -r`/build

INS=hello #Write your name of module without extension

.PHONY: default clean ins rm

default: #default build
	$(MAKE) -C $(KDIR) M=$$PWD

clean: #clean unecessary files
	$(MAKE) -C $(KDIR) M=$$PWD clean

ins: default #buliding and installing module into kernel
	sudo insmod "${INS}.ko"

rm: #deleting module from kernel 
	sudo rmmod ${INS} 

endif
