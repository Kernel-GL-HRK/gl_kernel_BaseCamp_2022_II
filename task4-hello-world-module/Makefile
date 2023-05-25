obj-m :=hello.o
CFLAGS_hello.o := -DDEBUG

export ARCH = arm
export CROSS_COMPILE = arm-linux-gnueabihf-
export KDIR = ~/gl_base_camp/linux_kernel_arm/kernel

PWD := $(CURDIR)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
