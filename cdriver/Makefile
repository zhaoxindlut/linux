ifneq ($(KERNELRELEASE),)
obj-m +=driver_insmod.o
else
PWD :=$(shell pwd)
KVER :=$(shell uname -r)
KDIR :=/lib/modules/$(KVER)/build
all:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	rm -rf *.o  *.mod.c *.symvers *.order *.markers *~
endif
