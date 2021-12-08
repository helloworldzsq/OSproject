ifeq ($(KERNELRELEASE),)

KERNELDIR ?= /usr/src/linux-headers-5.11.0-40-generic

PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.markers *.mod.c *.mod.o *.symvers .tmp_versions

.PHONY: modules modules_install clean

else
    obj-m := mem_dev.o
endif