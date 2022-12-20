CC=arm-buildroot-linux-uclibcgnueabi-gcc
INLCUDE_DIR=../../hw
all: uio 

uio: uio.c 
	$(CC) -o $@ $^ -I $(INLCUDE_DIR)

clean:
	rm -f *.o
	rm -f uio
