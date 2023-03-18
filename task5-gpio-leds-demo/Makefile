
obj-m := gpio_leds_rgb.o

gpio_leds_rgb-m := \
	leds-rgb.o \
	leds-rgb-control.o \
	leds-rgb-procfs.o 

EXTRA_CFLAGS += -Wno-unused-variable -Wno-unused-result

KDIR ?= ../../../kernel_old/linux/
ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-

default:
	$(MAKE) -C $(KDIR) M=$$PWD CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) modules

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

.PHONY: clean default
