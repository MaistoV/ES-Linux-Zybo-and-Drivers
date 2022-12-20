obj-m+=hellok.o

#VERSION = $(shell uname -r)
#KERNEL_SOURCE = /lib/modules/$(VERSION)/build M=$(shell pwd) modules

PWD := $(shell pwd)
KERNEL_SOURCE=${PWD}/../../../Linux_Zybo/work/linux-digilent/

default:
#	make -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	make -C ${KERNEL_SOURCE} M=${PWD} modules


clean:
#	make -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) clean
	make -C $(KERNEL_SOURCE) M=$(PWD) clean
