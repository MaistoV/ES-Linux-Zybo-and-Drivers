CC = arm-buildroot-linux-uclibcgnueabi-gcc
INLCUDE_DIR=../../hw
all: kernel_device 

kernel_device: kernel_device.c 
	$(CC) -o $@ $^ -I $(INLCUDE_DIR)

clean:
	rm -f *.o
	rm -f kernel_device
