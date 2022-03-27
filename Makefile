
obj-m := pstraverse_module.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules	
clean: 
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean

