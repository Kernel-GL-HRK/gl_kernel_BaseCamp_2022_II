CC = gcc
CFLAGS = -Wall -pedantic -g
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

rotary-objs := rotary.o procfs.o sysfs.o ioctl.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

rotary.o: rotary.c libs.h globals.h procfs.h sysfs.h ioctl.h procfs.c sysfs.c ioctl.c
	$(CC) $(CFLAGS) -c rotary.c procfs.c sysfs.c ioctl.c

procfs.o: procfs.c procfs.h globals.h
	$(CC) $(CFLAGS) -c $<

sysfs.o: sysfs.c sysfs.h
	$(CC) $(CFLAGS) -c $<

ioctl.o: ioctl.c ioctl.h globals.h
	$(CC) $(CFLAGS) -c $<

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean