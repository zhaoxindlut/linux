ifneq ($(KERNELRELEASE),)
# mymodule-objs := hello.c
obj-m += hello.o

else
PWD :=$(shell pwd)
KVER :=$(shell uname -r)
KDIR :=/lib/modules/$(KVER)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	rm -rf *.o *.ko *.mod.c *.symvers *.order *.markers *~ 
endif
