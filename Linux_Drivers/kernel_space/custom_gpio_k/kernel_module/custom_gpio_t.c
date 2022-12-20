#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include "custom_gpio_t.h"

/* Initialization of struct custom_gpio */
int custom_gpio_Init(custom_gpio_t *custom_gpio_device,
                     struct module *owner,
                     struct platform_device *plat_dev,
                     struct class *class,
                     const char *driver_name,
                     const char *device_name,
                     struct file_operations *f_ops,
                     irq_handler_t irq_handler)
{

    int error = 0;
    char *file_name;
    struct device *dev = NULL;

    printk(KERN_ERR "Call to %s\n", __func__);

    // Allocate kernel memory for the device name string
    // GFP_KERNEL is used to allocate sized smaller than the page size
    file_name = NULL;
    if ((file_name = kmalloc(strlen(driver_name), GFP_KERNEL)) == NULL)
    {
        error = -ENOMEM;
        return error;
    }
    // Write this string in memory
    sprintf(file_name, device_name);
    sprintf(custom_gpio_device->name, file_name);
    printk(KERN_INFO "%s: Registering device %s\n", __func__, file_name);

    // Field initializzations
    custom_gpio_device->plat_dev = plat_dev;
    custom_gpio_device->class = class;

    // Request minor and major number to kernel
    if ((error = alloc_chrdev_region(&(custom_gpio_device->Mm), 0, 1, file_name)) != 0)
    {
        printk(KERN_ERR "%s: alloc_chrdev_region() returned %d\n", __func__, error);
        return error;
    }

    // Initialize a charcter device, binding the file operations
    cdev_init(&custom_gpio_device->cdev, f_ops);

    // The owner filed must be initialized explicitly
    custom_gpio_device->cdev.owner = owner;

    // Create character device
    if ((custom_gpio_device->dev = device_create(class, NULL, custom_gpio_device->Mm, NULL, file_name)) == NULL)
    {
        printk(KERN_ERR "%s: device_create() returned NULL\n", __func__);
        error = -ENOMEM;
        goto device_create_error;
    }

    // Add character device
    if ((error = cdev_add(&custom_gpio_device->cdev, custom_gpio_device->Mm, 1)) != 0)
    {
        printk(KERN_ERR "%s: cdev_add() returned %d\n", __func__, error);
        goto cdev_add_error;
    }

    /**
     * Use the macro of_address_to_resource to populate the resurce structure, which represent
     * the physical memory layout information of the device
     */
    dev = &(plat_dev->dev);
    printk(KERN_INFO "%s: Parsing device tree for %s\n", __func__, dev->of_node->full_name);
    if ((error = of_address_to_resource(dev->of_node, 0, &custom_gpio_device->rsrc)) != 0)
    {
        printk(KERN_ERR "%s: of_address_to_resource() returned %d\n", __func__, error);
        goto of_address_to_resource_error;
    }
    // Compute and save the memory size
    custom_gpio_device->rsrc_size = custom_gpio_device->rsrc.end - custom_gpio_device->rsrc.start + 1;

    // Allocate virtual memory for the device
    if ((custom_gpio_device->mreg = request_mem_region(custom_gpio_device->rsrc.start, custom_gpio_device->rsrc_size, file_name)) == NULL)
    {
        printk(KERN_ERR "%s: request_mem_region() returned NULL\n", __func__);
        error = -ENOMEM;
        goto request_mem_region_error;
    }

    // Remap the allocated memory to make it visible to the kernel
    if ((custom_gpio_device->virtual_addr = ioremap(custom_gpio_device->rsrc.start, custom_gpio_device->rsrc_size)) == NULL)
    {
        printk(KERN_ERR "%s: ioremap() returned NULL\n", __func__);
        error = -ENOMEM;
        goto ioremap_error;
    }

    // Retrieve the irq line number from the device tree
    custom_gpio_device->irq_num = irq_of_parse_and_map(dev->of_node, 0);
    printk(KERN_INFO "%s:  irq_of_parse_and_map() returned %d\n",__func__, custom_gpio_device->irq_num);
    // Request an handler for this line
    if ((error = request_irq(custom_gpio_device->irq_num, irq_handler, 0, file_name, (void *)custom_gpio_device)) != 0)
    {
        printk(KERN_ERR "%s: request_irq() returned %d\n", __func__, error);
        goto irq_of_parse_and_map_error;
    }

    // Deallocate the buffer used for the string
    kfree(file_name);

    // Initialize wait_queues
    init_waitqueue_head(&custom_gpio_device->read_queue);
    init_waitqueue_head(&custom_gpio_device->poll_queue);

    // Initialize spinlocks
    spin_lock_init(&custom_gpio_device->slock_can_read);
    spin_lock_init(&custom_gpio_device->slock_tot_interrupts);
    spin_lock_init(&custom_gpio_device->slock_mode_reg);
    spin_lock_init(&custom_gpio_device->slock_int_en_reg);

    // Initialize internal variables
    custom_gpio_device->can_read = 0;
    custom_gpio_device->total_interrupts = 0;

    // Printing the resulting structures
    printk(KERN_INFO "%s: custom_gpio_t* custom_gpio_device;	 %p\n", __func__, custom_gpio_device                        );                 
    printk(KERN_INFO "%s: char name[27];                         %s\n", __func__, custom_gpio_device->name                  );       
    printk(KERN_INFO "%s: struct platform_device *plat_dev;	     %p\n", __func__, custom_gpio_device->plat_dev              );       
    printk(KERN_INFO "%s: dev_t Mm;							     %d\n", __func__, custom_gpio_device->Mm                    );             
    printk(KERN_INFO "%s: struct cdev cdev;					     %d\n", __func__, custom_gpio_device->cdev                  );           
    printk(KERN_INFO "%s: struct device *dev;                    %p\n", __func__, custom_gpio_device->dev                   );            
    printk(KERN_INFO "%s: struct class *class;                   %p\n", __func__, custom_gpio_device->class                 );          
    printk(KERN_INFO "%s: int irq_num;			                 %d\n", __func__, custom_gpio_device->irq_num               );        
    printk(KERN_INFO "%s: struct resource rsrc.start             %d\n", __func__, custom_gpio_device->rsrc.start            );           
    printk(KERN_INFO "%s: struct resource rsrc.end               %d\n", __func__, custom_gpio_device->rsrc.end              );           
    printk(KERN_INFO "%s: struct resource *mreg                  %p\n", __func__, custom_gpio_device->mreg                  );           
    printk(KERN_INFO "%s: uint32_t rsrc_size;	                 %d\n", __func__, custom_gpio_device->rsrc_size             );      
    printk(KERN_INFO "%s: void *virtual_addr;	                 %p\n", __func__, custom_gpio_device->virtual_addr          );   
    printk(KERN_INFO "%s: wait_queue_head_t read_queue;          %d\n", __func__, custom_gpio_device->read_queue            );     
    printk(KERN_INFO "%s: wait_queue_head_t poll_queue;          %d\n", __func__, custom_gpio_device->poll_queue            );     
    printk(KERN_INFO "%s: uint32_t can_read;                     %d\n", __func__, custom_gpio_device->can_read              );       
    printk(KERN_INFO "%s: spinlock_t slock_can_read;             %d\n", __func__, custom_gpio_device->slock_can_read        );     
    printk(KERN_INFO "%s: uint32_t total_interrupts;             %d\n", __func__, custom_gpio_device->total_interrupts      );
    printk(KERN_INFO "%s: spinlock_t slock_tot_interrupts;       %d\n", __func__, custom_gpio_device->slock_tot_interrupts  );
    printk(KERN_INFO "%s: spinlock_t slock_mode_reg;             %d\n", __func__, custom_gpio_device->slock_mode_reg        );
    printk(KERN_INFO "%s: spinlock_t slock_int_en_reg;           %d\n", __func__, custom_gpio_device->slock_int_en_reg      );


    // Reset the device
    custom_gpio_Reset(custom_gpio_device);


    goto no_error;

// Error handling
irq_of_parse_and_map_error:
    iounmap(custom_gpio_device->virtual_addr);

ioremap_error:
    release_mem_region(custom_gpio_device->rsrc.start, custom_gpio_device->rsrc_size);

request_mem_region_error:

of_address_to_resource_error:

cdev_add_error:
    device_destroy(custom_gpio_device->class, custom_gpio_device->Mm);

device_create_error:
    cdev_del(&custom_gpio_device->cdev);
    unregister_chrdev_region(custom_gpio_device->Mm, 1);

no_error:
    return error;
}

// Deallocating all the allocated structures 
void custom_gpio_Destroy(custom_gpio_t *device)
{

    // Disable interrupts
    custom_gpio_GlobalInterruptDisable(device);

    // Free the irq line
    free_irq(device->irq_num, device);

    // Remove virtual address mapping
    iounmap(device->virtual_addr);

    // Release device virtual memory region
    release_mem_region(device->rsrc.start, device->rsrc_size);

    // Destroy device
    device_destroy(device->class, device->Mm);

    // Unregister char device
    cdev_del(&device->cdev);

    // Release Major/Minor number
    unregister_chrdev_region(device->Mm, 1);
}

void custom_gpio_IncrementTotalInterrupt(custom_gpio_t *device)
{
    unsigned long flags;

    spin_lock_irqsave(&device->slock_tot_interrupts, flags);

    device->total_interrupts++;

    spin_unlock_irqrestore(&device->slock_tot_interrupts, flags);

}

uint32_t custom_gpio_GetTotalInterrupt(custom_gpio_t *device)
{
    unsigned long flags;
    uint32_t return_value;

    spin_lock_irqsave(&device->slock_tot_interrupts, flags);
    return_value = device->total_interrupts;
    spin_unlock_irqrestore(&device->slock_tot_interrupts, flags);

    return return_value;
}

void* custom_gpio_GetDeviceAddress(custom_gpio_t *device)
{
    return device->virtual_addr;
}

void custom_gpio_GlobalInterruptEnable(custom_gpio_t *device)
{
    unsigned long flags;

    spin_lock_irqsave(&device->slock_int_en_reg, flags);

    iowrite32(CUSTOM_GPIO_INT_EN_ENABLE, (device->virtual_addr + CUSTOM_GPIO_INT_EN_REG_OFFSET));

    spin_unlock_irqrestore(&device->slock_int_en_reg, flags);
}

void custom_gpio_GlobalInterruptDisable(custom_gpio_t *device)
{
    unsigned long flags;

    spin_lock_irqsave(&device->slock_int_en_reg, flags);

    iowrite32(CUSTOM_GPIO_INT_EN_DISABLE, (device->virtual_addr + CUSTOM_GPIO_INT_EN_REG_OFFSET));

    spin_unlock_irqrestore(&device->slock_int_en_reg, flags);
}

extern uint32_t custom_gpio_GetInterruptEnable(custom_gpio_t *device)
{
    uint32_t status_reg;
    unsigned long flags;

    spin_lock_irqsave(&device->slock_int_en_reg, flags);

    status_reg = ioread32(device->virtual_addr + CUSTOM_GPIO_INT_EN_REG_OFFSET);

    spin_unlock_irqrestore(&device->slock_int_en_reg, flags);

    return status_reg;
}

extern void custom_gpio_SetInterruptEnable(custom_gpio_t *device, uint32_t value)
{
    unsigned long flags;

    spin_lock_irqsave(&device->slock_int_en_reg, flags);

    iowrite32(value, (device->virtual_addr + CUSTOM_GPIO_INT_EN_REG_OFFSET));

    spin_unlock_irqrestore(&device->slock_int_en_reg, flags);
}

void custom_gpio_Reset(custom_gpio_t *device)
{
    unsigned long flags;

    spin_lock_irqsave(&device->slock_mode_reg, flags);

    iowrite32(CUSTOM_GPIO_MODE_READ, (device->virtual_addr + CUSTOM_GPIO_MODE_REG_OFFSET));

    spin_unlock_irqrestore(&device->slock_mode_reg, flags);

    custom_gpio_GlobalInterruptDisable(device);

}

uint32_t custom_gpio_GetPendingInterrupt(custom_gpio_t *device)
{
    uint32_t return_value;

    // This register is read only, there is no need to use spinlocks 
    return_value = ioread32((device->virtual_addr + CUSTOM_GPIO_PENDING_INT_REG_OFFSET));

    return return_value;
}


void custom_gpio_SetCanRead(custom_gpio_t *device)
{
    unsigned long flags;
    spin_lock_irqsave(&device->slock_can_read, flags);
    device->can_read = 1;
    spin_unlock_irqrestore(&device->slock_can_read, flags);
}

void custom_gpio_TestCanReadAndSleep(custom_gpio_t *device)
{
    wait_event_interruptible(device->read_queue, (device->can_read != 0));
}

void custom_gpio_ResetCanRead(custom_gpio_t *device)
{
    unsigned long flags;
    spin_lock_irqsave(&device->slock_can_read, flags);
    device->can_read = 0;
    spin_unlock_irqrestore(&device->slock_can_read, flags);
}

void custom_gpio_WakeUpRead(custom_gpio_t *device)
{
    wake_up_interruptible(&(device->read_queue));
}

unsigned custom_gpio_GetPollMask(custom_gpio_t *device, struct file *file_ptr, struct poll_table_struct *poll_table)
{
    unsigned mask = 0;

    // Sync the poll table
    poll_wait(file_ptr, &(device->poll_queue), poll_table);

    // Can't be interrupted
    spin_lock(&device->slock_tot_interrupts);

    if (device->can_read)
        // There is something to read
        mask = POLLIN | POLLRDNORM;

    spin_unlock(&device->slock_tot_interrupts);

    return mask;
}
