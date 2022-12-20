TARGET = custom_gpio_kernel_driver
OBJS = custom_gpio_main.o custom_gpio_t.o

obj-m += $(TARGET).o
$(TARGET)-y += $(OBJS)
PWD := $(shell pwd)
KERNEL_SOURCE=${PWD}/../../../../Linux_Zybo/work/linux-digilent/


default:
#	make -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	make -C ${KERNEL_SOURCE} M=${PWD} modules


clean:
#	make -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) clean
	make -C $(KERNEL_SOURCE) M=$(PWD) clean
