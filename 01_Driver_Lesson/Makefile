# kbuild part of makefile
obj-m += hello_calc.o

# normal makefile
KDIR = /lib/modules/$(shell uname -r)/build

all:
        make -Wall -C $(KDIR)  M=$(shell pwd) modules

clean:
        make -C $(KDIR)  M=$(shell pwd) clean
\ No newline at end of file