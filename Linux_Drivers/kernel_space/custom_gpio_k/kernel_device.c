
#include <inttypes.h> 	// For uint32_t
#include <stdio.h> 		// For stdin and stdout
#include <stdlib.h> 	// For strtoul
#include <unistd.h> 	// For sysconf and file operations
#include <sys/mman.h> 	// For mmap
#include <fcntl.h> 		// For mmap
#include <string.h> 	// For strcmp
#include "custom_gpio_memory_map.h" 

void print_help(char** argv){
	printf("Wrong number of argumetns. Example of use:\n%s led value (write a value to led[3:0])\n%s sw [nonblock](read value from sw[3:0])\n",argv[0],argv[0]);
}

#define GPIO_OP_NONE  0
#define GPIO_OP_READ  1
#define GPIO_OP_WRITE 2

int main(int argc, char** argv) {
	uint32_t gpio_addr, page_size, page_mask, page_addr, offset;
	uint32_t* custom_gpio_mode_reg;
	uint32_t mode_value, gpio_value;
	uint8_t gpio_op = GPIO_OP_NONE;
	int fd;
	void* vrt_page_addr;
	void* vrt_gpio_addr;
	char dev_filename[32] = "/dev/custom_gpio_device_unknown";
	uint32_t flags =  O_RDWR;

	switch (argc){
	case 2:
		if ( strcmp(argv[1],"sw") != 0 ) {
			print_help(argv);
			return 0;
		}
		sprintf(dev_filename, "/dev/custom_gpio_device_sw");
		gpio_op = GPIO_OP_READ;
		flags =  O_RDWR;
		break;
	case 3:
		if ( strcmp(argv[1],"led") == 0 ) {
			gpio_op = GPIO_OP_WRITE;
			sprintf(dev_filename, "/dev/custom_gpio_device_led");
			gpio_value = strtoul(argv[2], NULL, 0) & 0xf;
		}
		else if ( strcmp(argv[1],"sw") == 0 ) {
			gpio_op = GPIO_OP_READ;
			sprintf(dev_filename, "/dev/custom_gpio_device_sw");
			flags = ( strcmp(argv[2],"nonblock") == 0 ) ? (O_RDWR | O_NONBLOCK) : O_RDWR;			
		}
		else {
			print_help(argv);
			return 0;
		}
		break;
	default:
		print_help(argv);
		return 0;
		break;
	}

    // Open the GPIO device as a file 
	fd = open (dev_filename, flags);
	if ( fd < 1 ) {
		printf("Impossible to open %s\n", dev_filename);
		return -1;
	}

	switch ( gpio_op )
	{
	case GPIO_OP_READ:
		// Enable read mode for all pins
		printf("Writing on mode reg \n");
        mode_value = CUSTOM_GPIO_MODE_READ;
		pwrite(fd, &mode_value, sizeof(uint32_t), CUSTOM_GPIO_MODE_REG_OFFSET);		
        // Read 
		printf("Reading from Data reg \n");
		pread(fd, &gpio_value, sizeof(uint32_t), CUSTOM_GPIO_DATA_REG_OFFSET);
		printf("Data reg value is %02x\n", gpio_value);
		break;
	case GPIO_OP_WRITE:
		// Enable write mode for all pins
        mode_value = CUSTOM_GPIO_MODE_WRITE;
		printf("Writing on mode reg 0x%02x\n", mode_value);
		pwrite(fd, &mode_value, sizeof(uint32_t), 4);
		// Write 
		printf("Writing on write reg 0x%02x\n", gpio_value);
		pwrite(fd, &gpio_value, sizeof(uint32_t), CUSTOM_GPIO_DATA_REG_OFFSET);
		break;	
	default:
		// Do nothing
		printf("No operation to perform on GPIO\n");
		break;
	}

    // Cleanup
	close(fd);
    
    return 0;
}