obj-m += hello_procfs.o
obj-m += led-gpio-proc.o

PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules  

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
