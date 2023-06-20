# Name of kernel module to be build
obj-m := smart_clock.o

# List of object file(s) to be built
smart_clock-m := \
	sc.o \
	sc_control.o \
	sc_button.o \
	sc_panel.o \
	sc_sensors.o

# Flags for the compiler to disable some not important warnings
EXTRA_CFLAGS += \
	-Wno-unused-result \
	-Wno-sequence-point \
	-Wno-unused-variable

# Default command line values for Makefile
KDIR ?= ../../kernel_old/linux/
ARCH = arm
CROSS_COMPILE ?= arm-linux-gnueabihf-



# Check if dtc is installed in kernel

ifneq (, $(wildcard $(KDIR)scripts/dtc/dtc))

# Assign dtc kernel path
DTC  = $(KDIR)scripts/dtc/dtc

# Else check if dtc is installed in system PATH
else ifeq (, $(shell which dtc))
$(error  No dtc bin found, consider doing apt-get install dtc or comiling kernel before)
else

# Assign dtc system path
DTC  =$(shell which dtc)
endif

default:dtbs
	$(MAKE) -C $(KDIR) M=$$PWD CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) modules

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

dtbs:
	$(DTC) -I dts -O dtb smart_clock.dts -o smart_clock.dtbo

help:
	@echo "Flags and variables:"
	@echo " 'KDIR'			path to kernel"
	@echo "			By Default ../../kernel_old/linux/"
	@echo " 'CROSS_COMPILE'	ARM crosscompiler"
	@echo "			By Default arm-linux-gnueabihf-"

.PHONY: clean dtbs
