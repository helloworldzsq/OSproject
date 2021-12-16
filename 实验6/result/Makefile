obj-m := rwbuf.o
KERNELDIR := /usr/src/linux-headers-5.11.0-40-generic
PWD := $(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order  Module.symvers
