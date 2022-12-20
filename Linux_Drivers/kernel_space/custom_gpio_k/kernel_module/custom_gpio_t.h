#ifndef __CUSTOM_GPIO_T__
#define __CUSTOM_GPIO_T__

#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#include <linux/wait.h>
#include <linux/spinlock.h>

#include <asm/uaccess.h>
#include <asm/io.h>

// Peripheral register offsets
#define CUSTOM_GPIO_DATA_REG_OFFSET    		0x0U
#define CUSTOM_GPIO_MODE_REG_OFFSET    		0x4U
#define CUSTOM_GPIO_INT_EN_REG_OFFSET  		0x8U
#define CUSTOM_GPIO_PENDING_INT_REG_OFFSET 	0xCU

// Mode register macros
#define CUSTOM_GPIO_MODE_READ           0X00000000
#define CUSTOM_GPIO_MODE_WRITE          0XFFFFFFFF
// Interrupt enable register macros
#define CUSTOM_GPIO_INT_EN_DISABLE      0X00000000
#define CUSTOM_GPIO_INT_EN_ENABLE       0XFFFFFFFF

typedef struct{
	char name[27];
	struct platform_device *plat_dev;
	
	dev_t Mm;									// Device major e minor number 
		
	struct cdev cdev;							// Char device structure
	struct device *dev;							// Device structure associated to the device
	struct class *class;						// Class structure associated to the device
	
	uint32_t irq_num;							// Interrupt line number
	
	struct resource rsrc;						// Device memory abstraction 
												
	struct resource *mreg;						// Pointer to the memory region on which the device is mapped
	
	uint32_t rsrc_size;							// Size of the memory region on which the device is mapped
	
	void *virtual_addr;							// Device virtual address
	
	wait_queue_head_t read_queue;				// wait_queue for the read()
	wait_queue_head_t poll_queue;				// wait_queue for the poll()

	uint32_t can_read;							
	spinlock_t slock_can_read;					// Spinlock for the can_read variable
	
	
	uint32_t total_interrupts;					// Interrupt counter
	spinlock_t slock_tot_interrupts;			// Spinlock for the total_interrupts variable
	
	spinlock_t slock_mode_reg;					// Spinlock for the access to the register slock_mode_reg variable
	spinlock_t slock_int_en_reg;				// Spinlock for the access to the register slock_int_en_reg variable
} custom_gpio_t;
	

int custom_gpio_Init(custom_gpio_t *custom_gpio_device,
                     struct module *owner,
                     struct platform_device *plat_dev,
                     struct class *class,
                     const char *driver_name,
                     const char *device_name,
                     struct file_operations *f_ops,
                     irq_handler_t irq_handler);
			  
extern void custom_gpio_Reset(custom_gpio_t *device);

extern void custom_gpio_Destroy(custom_gpio_t *device); 

extern void* custom_gpio_GetDeviceAddress(custom_gpio_t *device); 

extern void custom_gpio_TestCanReadAndSleep(custom_gpio_t* device);
extern void custom_gpio_WakeUpRead(custom_gpio_t* device);

extern void custom_gpio_SetCanRead(custom_gpio_t *device);
extern void custom_gpio_ResetCanRead(custom_gpio_t *device); 

extern void custom_gpio_IncrementTotalInterrupt(custom_gpio_t *device); 
extern uint32_t custom_gpio_GetTotalInterrupt(custom_gpio_t *device); 

extern void custom_gpio_GlobalInterruptEnable(custom_gpio_t* device); 
extern void custom_gpio_GlobalInterruptDisable(custom_gpio_t* device);  

extern uint32_t custom_gpio_GetInterruptEnable(custom_gpio_t* device);
extern void custom_gpio_SetInterruptEnable(custom_gpio_t* device, uint32_t value);

extern uint32_t custom_gpio_GetPendingInterrupt(custom_gpio_t* device); 

extern unsigned custom_gpio_GetPollMask(custom_gpio_t *device, struct file *file_ptr, struct poll_table_struct *wait);

#endif					


