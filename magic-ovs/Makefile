ifneq ($(KERNELRELEASE),)
	obj-m := nmovs.o
	nmovs-objs += main_nm08_ovs.o
else
	KERNELDIR ?=/lib/modules/$(shell uname -r)/build
	PWD :=$(shell pwd)
      default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
      clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
