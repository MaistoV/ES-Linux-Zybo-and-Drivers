#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/unistd.h>

#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/sched.h>
#include <linux/poll.h>

#include "custom_gpio_t.h"

#define DRIVER_NAME "custom_gpio_kernel_driver"
#define DRIVER_FNAME "custom_gpio_device_"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vincenzo Maisto <vincenzo.maisto2@unina.it>");
MODULE_DESCRIPTION("custom_gpio kernel-mode driver");
MODULE_VERSION("1.0");
MODULE_ALIAS(DRIVER_NAME);

/* Function implemented for the device. The first two functions are specific for platform devices */
static int custom_gpio_probe(struct platform_device *plat_dev);
static int custom_gpio_remove(struct platform_device *plat_dev);
static int custom_gpio_open(struct inode *inode, struct file *file_ptr);
static int custom_gpio_release(struct inode *inode, struct file *file_ptr);
static loff_t custom_gpio_llseek(struct file *file_ptr, loff_t off, int whence);
static ssize_t custom_gpio_read(struct file *file_ptr, char *buf, size_t count, loff_t *ppos);
static ssize_t custom_gpio_write(struct file *file_ptr, const char *buf, size_t size, loff_t *off);
static irqreturn_t custom_gpio_irq_handler(int irq, void *dev_id);
static unsigned int custom_gpio_poll(struct file *file_ptr, struct poll_table_struct *wait);

/* Class associated with the device */
static struct class *custom_gpio_class = NULL;

/* Bind device in device tree */
static struct of_device_id custom_gpio_match[] = {
	{.compatible = DRIVER_NAME},
	{},
};

/* Bind probe and remove functions to device driver */
static struct platform_driver custom_gpio_driver = {
	.probe = custom_gpio_probe,
	.remove = custom_gpio_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = custom_gpio_match,
	},
};

/* Add a new entry to device table */
MODULE_DEVICE_TABLE(of, custom_gpio_match);

/* Macro to automatically implement module_init() and module_exit() */
module_platform_driver(custom_gpio_driver);

/* Bind file operations to custom implementations */
static struct file_operations custom_gpio_fops = {
	.owner = THIS_MODULE,
	.llseek = custom_gpio_llseek,
	.read = custom_gpio_read,
	.write = custom_gpio_write,
	.poll = custom_gpio_poll,
	.open = custom_gpio_open,
	.release = custom_gpio_release
};

/* Gloabal pointer ot file descriptor */
custom_gpio_t *custom_gpio_ptr_global_sw = NULL;
custom_gpio_t *custom_gpio_ptr_global_led = NULL;
custom_gpio_t *custom_gpio_ptr_global_unknown = NULL;

uint32_t device_count = 0;

static int custom_gpio_probe(struct platform_device *plat_dev)
{
	int error = 0;
	custom_gpio_t *custom_gpio_ptr = NULL;
	char device_file_name[27] = DRIVER_FNAME;
	printk(KERN_INFO "Call to %s\n", __func__);

	// Create a device class, just once
	if (device_count == 0){
		if ((custom_gpio_class = class_create(THIS_MODULE, DRIVER_NAME)) == NULL)
		{
			printk(KERN_ERR "%s: class_create() returned NULL\n", __func__);
			return -ENOMEM;
		}
	}

	// Allocate an instance of custom_gpio_t
	if ((custom_gpio_ptr = kmalloc(sizeof(custom_gpio_t), GFP_KERNEL)) == NULL)
	{
		printk(KERN_ERR "%s: kmalloc() returned NULL\n", __func__);
		return -ENOMEM;
	}

	// Keep track of registered devices
	if (device_count == 0) {
		strcat(device_file_name, "led");
		device_count++;
	}
	else if (device_count == 1)	{
		strcat(device_file_name, "sw");
		device_count++;
	}
	else {
		strcat(device_file_name, "unknown");
	}

	// Init device driver
	if ((error = custom_gpio_Init(	custom_gpio_ptr, 
									THIS_MODULE,
									plat_dev, 
									custom_gpio_class,
									DRIVER_NAME, 
									device_file_name, 
									&custom_gpio_fops, 
									(irq_handler_t)custom_gpio_irq_handler)) != 0)
	// if ((error = custom_gpio_Init(custom_gpio_ptr, THIS_MODULE, plat_dev, custom_gpio_class, DRIVER_NAME, DRIVER_FNAME, &custom_gpio_fops, (irq_handler_t)custom_gpio_irq_handler)) != 0)
	{
		printk(KERN_ERR "%s: custom_gpio_Init() returned %d\n", __func__, error);
		kfree(custom_gpio_ptr);
		return error;
	}

	// Initialize global variables
	if (device_count == 1) {
		custom_gpio_ptr_global_led = custom_gpio_ptr;
	}
	else if (device_count == 2) {
		custom_gpio_ptr_global_sw = custom_gpio_ptr;
	}
	else {
		printk(KERN_ERR "%s: This driver only supports up to two custom_gpio devices. \n\tRun rmmod custom_gpio_kernel_driver. \n\tYou might want to add support for more devices as an exercise\n", __func__);
		custom_gpio_ptr_global_unknown = custom_gpio_ptr;
	}

	return error;
}

static int custom_gpio_remove(struct platform_device *plat_dev_local)
{
	custom_gpio_t *custom_gpio_ptr = NULL;

	printk(KERN_INFO "Call to %s\n", __func__);

	if (plat_dev_local == NULL)
	{
		printk(KERN_WARNING "%s: plat_dev_local is NULL\n", __func__);
		return -1;
	}

	// Fetch pointer to global structure
	if (device_count == 1) {
		custom_gpio_ptr = custom_gpio_ptr_global_led;
	}
	else if (device_count == 2) {
		custom_gpio_ptr = custom_gpio_ptr_global_sw;
	}
	else {
		custom_gpio_ptr = custom_gpio_ptr_global_unknown;
	}

	// Clean-up
	if (custom_gpio_ptr != NULL){
		printk(KERN_INFO "%s: Removing device %s\n", __func__, custom_gpio_ptr->name);

		custom_gpio_Destroy(custom_gpio_ptr);
		printk(KERN_INFO "%s: custom_gpio_ptr destroyed\n", __func__);

		kfree(custom_gpio_ptr);
		printk(KERN_INFO "%s: custom_gpio_ptr memory freed\n", __func__);

		// NULL-out unused pointers
		custom_gpio_ptr_global_unknown = NULL;
		device_count--;
		if ( device_count == 1 ) {
			custom_gpio_ptr_global_sw = NULL;
		}
		else if ( device_count == 0 ) {
			custom_gpio_ptr_global_led = NULL;
		}
	}
	else {
		printk(KERN_WARNING "%s: custom_gpio_ptr is NULL, doing no resource releasing\n", __func__);
	}

	// On last device removal, also destroy the
	if ( device_count == 0) {
		class_destroy(custom_gpio_class);
		printk(KERN_INFO "%s: Class destroyed\n", __func__);
	}

	return 0;
}

static int custom_gpio_open(struct inode *inode, struct file *file_ptr)
{
	custom_gpio_t *custom_gpio_ptr = NULL;

	printk(KERN_INFO "Call to %s\n", __func__);

	// Use the container_of macro to retrieve pointer custom_gpio_t instance starting from cdev
	custom_gpio_ptr = container_of(inode->i_cdev, custom_gpio_t, cdev);

	if (custom_gpio_ptr == NULL)
	{
		printk(KERN_INFO "%s: container_of() returned NULL\n", __func__);
		return -1;
	}

	printk(KERN_INFO "%s: Device name %s\n", __func__, custom_gpio_ptr->name);

	file_ptr->private_data = custom_gpio_ptr;

	// Device-specific operations
	custom_gpio_Reset(custom_gpio_ptr);

	return 0;
}

static int custom_gpio_release(struct inode *inode, struct file *file_ptr)
{
	printk(KERN_INFO "Call to %s\n", __func__);

	// Nothing to do

	return 0;
}

static loff_t custom_gpio_llseek(struct file *file_ptr, loff_t off, int whence)
{
	custom_gpio_t *custom_gpio_ptr;
	loff_t newpos;

	printk(KERN_INFO "Call to %s\n", __func__);

	custom_gpio_ptr = file_ptr->private_data;
	switch (whence)
	{
	case 0: /* SEEK_SET */
		newpos = off;
		break;
	case 1: /* SEEK_CUR */
		newpos = file_ptr->f_pos + off;
		break;
	case 2: /* SEEK_END */
		newpos = custom_gpio_ptr->rsrc_size + off;
		break;
	default: /* can't happen */
		return -EINVAL;
	}
	if (newpos < 0)
		return -EINVAL;
	file_ptr->f_pos = newpos;
	return newpos;
}

static unsigned int custom_gpio_poll(struct file *file_ptr, struct poll_table_struct *wait)
{
	custom_gpio_t *custom_gpio_dev;
	printk(KERN_INFO "Call to %s\n", __func__);
	custom_gpio_dev = file_ptr->private_data;
	return custom_gpio_GetPollMask(custom_gpio_dev, file_ptr, wait);

	return 0;
}

static ssize_t custom_gpio_write(struct file *file_ptr, const char *buf, size_t size, loff_t *off)
{
	custom_gpio_t *custom_gpio_ptr;
	uint32_t data_to_write;
	void *write_addr;

	printk(KERN_INFO "Call to %s\n", __func__);

	// Get the custom_gpio struct pointer
	custom_gpio_ptr = file_ptr->private_data;

	// Check that the offset does not exceed the reade
	if (*off > custom_gpio_ptr->rsrc_size)
		return -EFAULT;

	// Copy data to kernel-space
	if (copy_from_user(&data_to_write, buf, size))
	{
		return -EFAULT;
	}

	// Retrieve write address and add offset
	write_addr = custom_gpio_GetDeviceAddress(custom_gpio_ptr) + *off;
	printk(KERN_INFO "%s: write_addr is 0x%8x\n", __func__, write_addr);

	// Perform the write operation
	iowrite32(data_to_write, write_addr);

	return size;
}

static ssize_t custom_gpio_read(struct file *file_ptr, char *buf, size_t count, loff_t *off)
{
	custom_gpio_t *custom_gpio_ptr;
	void *read_addr;
	uint32_t data_read;

	printk(KERN_INFO "Call to %s\n", __func__);

	// Get the struct pointer
	custom_gpio_ptr = file_ptr->private_data;

	// Check that the offset does not exceed the reade
	if (*off > custom_gpio_ptr->rsrc_size)
		return -EFAULT;

	// Check for blocking behaviour
	if ((file_ptr->f_flags & O_NONBLOCK) == 0)
	{
		printk(KERN_INFO "%s: read() is blocking\n", __func__);
		printk(KERN_INFO "%s: Enabling interrupts\n", __func__);
		custom_gpio_GlobalInterruptEnable(custom_gpio_ptr);
		// Wait on the wait queue for reads
		// The irq_hanled will set the flag and wake up this process
		custom_gpio_TestCanReadAndSleep(custom_gpio_ptr);

		printk(KERN_INFO "%s: Woken up\n", __func__);

		// Reset the can_read flag
		custom_gpio_ResetCanRead(custom_gpio_ptr);
	}
	else
	{
		printk(KERN_INFO "%s: read() is not blocking\n", __func__);
	}

	// Retrieve read address and add offset
	read_addr = custom_gpio_GetDeviceAddress(custom_gpio_ptr) + *off;

	// Perform the read from device memory to kernel-space
	printk(KERN_INFO "%s: Reading from virtual address 0x%8x\n", __func__, read_addr);
	data_read = (uint32_t)ioread32(read_addr);

	// Copying data to user space
	if (copy_to_user(buf, &data_read, count))
	{
		return -EFAULT;
	}

	return count;
}

static irqreturn_t custom_gpio_irq_handler(int irq, void *dev_id)
{
	custom_gpio_t *custom_gpio_ptr = NULL;
	uint32_t pending_int_reg;
	irqreturn_t return_value;
	uint32_t data_read;

	printk(KERN_INFO "Call to %s, line: %d\n", __func__, irq);

	// Get device struct pointer
	custom_gpio_ptr = (custom_gpio_t *)dev_id;
	if (custom_gpio_ptr == NULL)
		return -1;
	printk(KERN_INFO "%s: Device name: %s\n", __func__, custom_gpio_ptr->name);

	// Read PENDING_INT_REG
	pending_int_reg = custom_gpio_GetPendingInterrupt(custom_gpio_ptr);
	printk(KERN_INFO "%s: Pending interrupt register value is 0x%8x\n", __func__, pending_int_reg);

	// Disable all interrupts
	custom_gpio_GlobalInterruptDisable(custom_gpio_ptr);

	// No pending interrupt
	if (pending_int_reg == 0)
	{
		printk(KERN_WARNING "%s: Not handled interrupt n°%d\n", __func__, custom_gpio_GetTotalInterrupt(custom_gpio_ptr));

		// Something went wrong, reset the device
		custom_gpio_Reset(custom_gpio_ptr);

		return_value = IRQ_NONE;
	}

	// There are pending interrupts
	// Count up
	custom_gpio_IncrementTotalInterrupt(custom_gpio_ptr);

	// Set the can_read flag
	custom_gpio_SetCanRead(custom_gpio_ptr);

	// Wake-up those processes sleeping on the read_queue
	custom_gpio_WakeUpRead(custom_gpio_ptr);

	// Don't re-enable interrupts

	printk(KERN_INFO "%s: Handled the interrupt n°%d \n", __func__, custom_gpio_GetTotalInterrupt(custom_gpio_ptr));

	return_value = IRQ_HANDLED;

	return return_value;
}
