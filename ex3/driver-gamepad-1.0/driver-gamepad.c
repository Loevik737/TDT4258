/*
 * This is a Linux kernel module for interfacing with the TDT4258 game pad.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define MODNAME "gamepad"

#define GPIO_PC_BASE 0x40006048

#define GPIO_CTRL     (0x00)
#define GPIO_MODEL    (0x04)
#define GPIO_MODEH    (0x08)
#define GPIO_DOUT     (0x0c)
#define GPIO_DOUTSET  (0x10)
#define GPIO_DOUTCLR  (0x14)
#define GPIO_DOUTTGL  (0x18)
#define GPIO_DIN      (0x1c)
#define GPIO_PINLOCKN (0x20)

struct gamepad_dev {
	struct cdev cdev;
	struct class *class;
	dev_t dev_id;
	void *gpio_memory;
};

static struct gamepad_dev device;

static int gamepad_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int gamepad_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	unsigned long err;
	unsigned char button_state;
	unsigned int gpio_din;

	gpio_din = ioread32((unsigned char*)device.gpio_memory + GPIO_DIN);	
	button_state = ~(gpio_din & 0xff);

	err = copy_to_user(buff, &button_state, 1);
	if (err != 0) {
		return -EFAULT;
	}

	return 1;
}

static ssize_t gamepad_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
	return -1;
}

static struct file_operations gamepad_fops = {
	.owner = THIS_MODULE,
	.read = gamepad_read,
	.write = gamepad_write,
	.open = gamepad_open,
	.release = gamepad_release,
};

static int __init gamepad_init(void)
{
	struct resource *gpio_memory_resource;
	int err;

	memset(&device, 0, sizeof(struct gamepad_dev));

	// Initialize chrdev
	err = alloc_chrdev_region(&device.dev_id, 0, 1, MODNAME);


	if (err != 0) {
		printk(KERN_WARNING MODNAME ": Could not allocate chrdev for gamepad. (%i)\n", err);
		return err;
	}

	cdev_init(&device.cdev, &gamepad_fops);
	device.cdev.owner = THIS_MODULE;
	device.cdev.ops = &gamepad_fops;


	// Setup GPIO
	gpio_memory_resource = request_mem_region(GPIO_PC_BASE, 9*sizeof(uint32_t), MODNAME);

	if (gpio_memory_resource == NULL) {
		printk(KERN_WARNING MODNAME ": Could not allocate GPIO registers.\n");
		return -1;
	}

	device.gpio_memory = ioremap_nocache(GPIO_PC_BASE, 9*sizeof(uint32_t));

	iowrite32(2, (unsigned char*)device.gpio_memory + GPIO_CTRL);	
	iowrite32(0x33333333, (unsigned char*)device.gpio_memory + GPIO_MODEL);	
	iowrite32(0xff, (unsigned char*)device.gpio_memory + GPIO_DOUT);	

	device.class = class_create(THIS_MODULE, MODNAME);
	device_create(device.class, NULL, device.dev_id, NULL, MODNAME);

	// Enable chrdev
	err = cdev_add(&device.cdev, device.dev_id, 1);

	if (err != 0) {
		printk(KERN_WARNING MODNAME ": Error %i while adding device.\n", err);
		return err;
	}

	printk(KERN_INFO MODNAME ": Initialized\n");

	return 0;
}

static void __exit gamepad_cleanup(void)
{
	cdev_del(&device.cdev);
	device_destroy(device.class, device.dev_id);
	class_destroy(device.class);
	unregister_chrdev_region(device.dev_id, 1);
	iounmap(device.gpio_memory);
	release_mem_region(GPIO_PC_BASE, 9*sizeof(uint32_t));

	printk(KERN_INFO MODNAME ": Cleanup complete\n");
}

module_init(gamepad_init);
module_exit(gamepad_cleanup);

MODULE_DESCRIPTION("Gamepad driver.");
MODULE_LICENSE("GPL");
