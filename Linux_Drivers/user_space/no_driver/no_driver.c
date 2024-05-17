
#include <inttypes.h> 	// For uint32_t
#include <stdio.h> 		// For stdin and stdout
#include <stdlib.h> 	// For strtoul
#include <unistd.h> 	// For sysconf and file operations
#include <sys/mman.h> 	// For mmap
#include <fcntl.h> 		// For mmap
#include <string.h> 	// For strcmp
#include "custom_gpio_memory_map.h" 

void print_help(char** argv){
	printf("Wrong number of arguments. Example of use:\n%s led value (write a value to led[3:0])\n%s sw (read value from sw[3:0]\n",argv[0],argv[0]);
}

#define GPIO_OP_NONE  0
#define GPIO_OP_READ  1
#define GPIO_OP_WRITE 2

int main(int argc, char** argv) {
	uint32_t gpio_phy_addr, page_size, page_mask, page_addr, offset;
	uint32_t* custom_gpio_mode_reg;
	uint32_t* custom_gpio_data_reg;
	uint8_t write_value, gpio_value;
	uint8_t gpio_op = GPIO_OP_NONE;
	int dev_mem_fd;
	void* vrt_page_addr;
	void* gpio_virt_addr;

	switch (argc){
	case 2:
		if ( strcmp(argv[1],"sw") != 0 ) {
			print_help(argv);
			return 0;
		}
		gpio_op	   = GPIO_OP_READ;
    	gpio_phy_addr  = CUSTOM_GPIO_SW_BASE;
		break;
	case 3:
		if ( strcmp(argv[1],"led") != 0 ) {
			print_help(argv);
			return 0;
		}
		gpio_op 	= GPIO_OP_WRITE;
	    gpio_phy_addr   = CUSTOM_GPIO_LED_BASE;
		write_value = strtoul(argv[2], NULL, 0) & 0xf;
		break;
	default:
		print_help(argv);
		return 0;
		break;
	}

    // Open the memory device as a file with read/write permissions
	dev_mem_fd = open ("/dev/mem", O_RDWR);
	if ( dev_mem_fd < 1 ) {
		printf("Impossible to open /dev/mem\n");
		return -1;
	}

	page_size = sysconf(_SC_PAGESIZE);	// Page size
	page_mask = ~(page_size-1);			// Page mask
	page_addr = gpio_phy_addr & page_mask;	// Physical page address

	// Request a memory mapping
	vrt_page_addr = mmap (	// Destination address 
							// (NULL -> let the kernel choose)
							NULL,
							// Page Size	
							page_size,
							// Read/Write permission
							PROT_READ | PROT_WRITE,
							// Share this mapping with other processes 
							//		-> Propagate the read/write operations
							MAP_SHARED,
							// /dev/mem file descriptor
							dev_mem_fd,
							// Physical page address 			
							page_addr 
						);			
	// Check for errors
	if ( vrt_page_addr == MAP_FAILED ) {
		printf("mmap FAILED!\n");
		return -1;
	}
	offset    = gpio_phy_addr - page_addr;	// Device offset from the physical page address
	printf("Virtual page address is 						0x%0x	\n", vrt_page_addr);
	gpio_virt_addr = vrt_page_addr + offset;	// indirizzo virtuale del device gpio
	printf("Virtual GPIO base address is 		 			0x%0x	\n", gpio_virt_addr);
	printf("Virtual DATA register base address is 			0x%08x	\n", gpio_virt_addr + CUSTOM_GPIO_DATA_REG_OFFSET	);
	printf("Virtual MODE register base address is			0x%08x	\n", gpio_virt_addr + CUSTOM_GPIO_MODE_REG_OFFSET 	);
	printf("Virtual INT_EN register base address is			0x%08x	\n", gpio_virt_addr + CUSTOM_GPIO_INT_EN_REG_OFFSET	);
	printf("Virtual PENDING_INT register base address is	0x%08x	\n", gpio_virt_addr + CUSTOM_GPIO_PENDING_INT_OFFSET	);

	custom_gpio_mode_reg = gpio_virt_addr + CUSTOM_GPIO_MODE_REG_OFFSET;
	custom_gpio_data_reg = gpio_virt_addr + CUSTOM_GPIO_DATA_REG_OFFSET;
	switch ( gpio_op ) {
	case GPIO_OP_READ:
		// Enable read mode for all pins
		*custom_gpio_mode_reg = CUSTOM_GPIO_MODE_READ;
		// Read
		// gpio_value = *((uint32_t*)(gpio_virt_addr + CUSTOM_GPIO_DATA_REG_OFFSET));
		gpio_value = *custom_gpio_data_reg;
		printf("Switches state is %02x\n", gpio_value);
		break;
	case GPIO_OP_WRITE:
		// Enable write mode for all pins
		*custom_gpio_mode_reg = CUSTOM_GPIO_MODE_WRITE; 
		// Write 
		printf("Writing on leds %02x\n", write_value);
		*custom_gpio_data_reg = write_value;
		// *((uint32_t*)(gpio_virt_addr + CUSTOM_GPIO_DATA_REG_OFFSET)) = write_value;
		break;	
	default:
		// Do nothing
		printf("No operation to perform on GPIO\n");
		break;
	}

    // Cleanup
	munmap(vrt_page_addr, page_size);
	close(dev_mem_fd);



    return 0;
}