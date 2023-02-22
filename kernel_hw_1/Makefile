obj-m := hello.o

KDIR ?= /lib/modules/`uname -r`/build
FILE ?= main.c

all:
	$(MAKE) -C $(KDIR) M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

check:
	$(KDIR)/scripts/checkpatch.pl --no-tree -f $(FILE)
