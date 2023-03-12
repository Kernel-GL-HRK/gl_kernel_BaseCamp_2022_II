obj-m := chrdev.o

KDIR ?= /home/kmakhno/Documents/rpi_sources/linux
FILE ?= main.c

all:
	$(MAKE) -C $(KDIR) M=$(PWD) mmodules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl --no-tree -f $(FILE)
