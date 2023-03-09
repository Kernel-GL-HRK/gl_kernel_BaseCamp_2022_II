obj-m += hello_module.o

KDIR = ~/Documents/linux_kernel_arm/kernel_1

PWD := $(CURDIR)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

