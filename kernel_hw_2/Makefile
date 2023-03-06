obj-m += hello.o

KDIR ?= /home/kmakhno/Documents/rpi_sources/linux

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
