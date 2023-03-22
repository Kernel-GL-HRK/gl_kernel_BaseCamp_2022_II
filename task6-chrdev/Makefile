ifneq ($(KERNELRELEASE),)
# Kbuild part of makefile
obj-m := chrdev.o
#CFLAGS_chrdev.o := -DDEBUG
else

# build for current kernel if no specified
KDIR ?= /lib/modules/`uname -r`/build

PWD := $(CURDIR)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
endif
