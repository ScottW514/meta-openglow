MODULE_NAME = openglow
OUT = $(MODULE_NAME).ko
SRC_DIR = ./src

KERNEL_SRC ?= /usr/src/kernel

SRC = leds.c lens.c io.c tmc2130.c thermal.c \
      cnc_buffer.c cnc.c cnc_api.c cnc_pins.c \
      openglow.c head_fans.c


ccflags-y += -I$(PWD)/include -Wno-unknown-pragmas

obj-m += $(MODULE_NAME).o
$(MODULE_NAME)-objs := $(foreach srcfile,$(SRC),$(SRC_DIR)/$(srcfile:.c=.o))

.PHONY: all clean modules_install

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean

# run menu-based config tool
config:
	$(MAKE) -$(MAKEFLAGS) -C $(KERNEL_SRC) nconfig
	cp $(KERNEL_SRC)/.config $(PWD)/config.new

# pass any other command to the kernel Makefile directly
%:
	$(MAKE) -$(MAKEFLAGS) -C $(KERNEL_SRC) $@
