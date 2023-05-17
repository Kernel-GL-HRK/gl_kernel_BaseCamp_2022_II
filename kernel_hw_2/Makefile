obj-m += hello.o

KDIR ?= /home/kmakhno/Documents/rpi_sources/linux
FILE ?= main.c

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl --no-tree -f $(FILE)
