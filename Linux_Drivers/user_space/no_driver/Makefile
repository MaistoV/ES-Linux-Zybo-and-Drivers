CC=arm-buildroot-linux-uclibcgnueabi-gcc
INLCUDE_DIR=../../hw
all: no_driver 

no_driver: no_driver.c 
	$(CC) -o $@ $^ -I $(INLCUDE_DIR)

clean:
	rm -f *.o
	rm -f no_driver
