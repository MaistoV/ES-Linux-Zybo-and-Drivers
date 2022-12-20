CC=arm-buildroot-linux-uclibcgnueabi-gcc
INLCUDE_DIR=../../hw
all: uio-int 

uio-int: uio-int.c 
	$(CC) -o $@ $^ -I $(INLCUDE_DIR)

clean:
	rm -f *.o
	rm -f uio
