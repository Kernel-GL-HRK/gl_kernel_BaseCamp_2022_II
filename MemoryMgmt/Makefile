KERNELDIR ?= ../../buildroot-2021.02.4/output/build/linux-5.10.7/ #WARNING relative path

obj-m := mempool.o

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
