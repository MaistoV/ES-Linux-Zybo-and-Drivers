
#include <inttypes.h> 	// For uint32_t
#include <stdio.h> 		// For stdin and stdout
#include <stdlib.h> 	// For strtoul
#include <unistd.h> 	// For sysconf and file operations
#include <sys/mman.h> 	// For mmap
#include <fcntl.h> 		// For mmap
#include <string.h> 	// For strcmp
#include "custom_gpio_memory_map.h" 

void print_help(char** argv){
	printf("Wrong number of argumetns. Example of use:\n%s led value (write a value to led[3:0])\n%s sw (read value from sw[3:0])\n",argv[0],argv[0]);
}

#define GPIO_OP_NONE  0
#define GPIO_OP_READ  1
#define GPIO_OP_WRITE 2

int main(int argc, char** argv) {
	uint32_t* custom_gpio_mode_reg;
	uint32_t* custom_gpio_data_reg;
	uint32_t* custom_gpio_int_en_reg;
    uint32_t page_size;
	uint8_t gpio_value, gpio_op;
	int uio_fd;
	void* gpio_virt_addr;
	char dev_filename [10] = "/dev/uio0";

	gpio_op = GPIO_OP_NONE;
	switch (argc){
	case 2:
		if ( strcmp(argv[1],"sw") != 0 ) {
			print_help(argv);
			return 0;
		}
		gpio_op = GPIO_OP_READ;
		sprintf(dev_filename, "/dev/uio1");
		break;
	case 3:
		if ( strcmp(argv[1],"led") == 0 ) {
			gpio_op = GPIO_OP_WRITE;
			sprintf(dev_filename, "/dev/uio0");
			gpio_value = strtoul(argv[2], NULL, 0) & 0xf;
		}
		else if ( strcmp(argv[1],"sw") == 0 ) {
			gpio_op = GPIO_OP_READ;
			sprintf(dev_filename, "/dev/uio1");
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

    // Open the uio device as a file with read/write permissions
	printf("Opening %s file\n", dev_filename);
	uio_fd = open (dev_filename, O_RDWR);
	if ( uio_fd < 1 ) {
		printf("Impossible to open %s\n", dev_filename);
		return -1;
	}

	// Request a memory mapping
	page_size = sysconf(_SC_PAGESIZE);		// Page Size
	gpio_virt_addr = mmap( 	NULL, 					// Destination address (NULL -> the kernel can choose)
							page_size, 				// Page Size
							PROT_READ | PROT_WRITE, // Read/Write permission
							MAP_SHARED, 			// Share this mapping with other processes -> Propagate the read/write operations
							uio_fd, 			    // /dev/uioX file descriptor
							0 );					// Virtual page address
	// Check for errors
	if ( gpio_virt_addr == MAP_FAILED ) {
		printf("mmap FAILED!\n");
		return -1;
	}
	
	printf("Virtual GPIO base address is		 			0x%0x	\n", gpio_virt_addr);
	printf("Virtual DATA register base address is 			0x%08x	\n", gpio_virt_addr + CUSTOM_GPIO_DATA_REG_OFFSET	);
	printf("Virtual MODE register base address is			0x%08x	\n", gpio_virt_addr + CUSTOM_GPIO_MODE_REG_OFFSET 	);
	printf("Virtual INT_EN register base address is			0x%08x	\n", gpio_virt_addr + CUSTOM_GPIO_INT_EN_REG_OFFSET	);
	printf("Virtual PENDING_INT register base address is	0x%08x	\n", gpio_virt_addr + CUSTOM_GPIO_PENDING_INT_OFFSET);

	custom_gpio_mode_reg 	= gpio_virt_addr + CUSTOM_GPIO_MODE_REG_OFFSET;
	custom_gpio_data_reg 	= gpio_virt_addr + CUSTOM_GPIO_DATA_REG_OFFSET;
	custom_gpio_int_en_reg  = gpio_virt_addr + CUSTOM_GPIO_INT_EN_REG_OFFSET;
	switch ( gpio_op )
	{
	case GPIO_OP_READ:
		// Enable read mode for all pins
		*custom_gpio_mode_reg 	= CUSTOM_GPIO_MODE_READ;
		// Enable device interrupts
		*custom_gpio_int_en_reg = CUSTOM_GPIO_GLOBAL_INT_ENABLE;

		// Use the system call read() do read the interrupt status
		// NOTE: the kernel interrupt handler will disable the interrupts for this uio file
		printf("Waiting for interrupt\n");
		uint32_t interrupt_count = 1;
		if ( read(uio_fd, &interrupt_count, sizeof(uint32_t)) != sizeof(uint32_t) ) {
			printf("Read error!\n");
			return -1;
		}
		printf("Interrupt count: %08x\n", interrupt_count);

		// Disable device interrupts
		*custom_gpio_int_en_reg = CUSTOM_GPIO_GLOBAL_INT_DISABLE;

		// Read from device
		gpio_value = *custom_gpio_data_reg;
		printf("Date reg state is %02x\n", gpio_value);

		// Re-enable uio file interrupts with the system call write()
		uint32_t reenable = 1;
		if ( write(uio_fd, (void*)&reenable, sizeof(uint32_t)) != sizeof(uint32_t) ) {
			printf("Write error!\n");
			return -1;
		}
		break;
	case GPIO_OP_WRITE:
		// Enable write mode for all pins
		*custom_gpio_mode_reg = CUSTOM_GPIO_MODE_WRITE; 
		// Write 
		printf("Writing on leds %02x\n", gpio_value);
		*custom_gpio_data_reg = gpio_value;
		// *((uint32_t*)(gpio_virt_addr + CUSTOM_GPIO_DATA_REG_OFFSET)) = write_value;
		break;	
	default:
		// Do nothing
		printf("No operation to perform on GPIO\n");
		break;
	}

    // Cleanup
	munmap(gpio_virt_addr, page_size);
	close(uio_fd);



    return 0;
}