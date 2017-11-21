#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fcntl.h>
#include <linux/signal.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <asm/ptrace.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define MODNAME "gamepad"

#define GPIO_BASE 0x40006000
#define GPIO_LENGTH 0x120

#define GPIO_EXTIPSELL (0x100)
#define GPIO_EXTIPSELH (0x104)
#define GPIO_EXTIRISE  (0x108)
#define GPIO_EXTIFALL  (0x10c)
#define GPIO_IEN       (0x110)
#define GPIO_IF        (0x114)
#define GPIO_IFC       (0x11c)

#define GPIO_PC_BASE 0x48
#define GPIO_CTRL     (0x00)
#define GPIO_MODEL    (0x04)
#define GPIO_MODEH    (0x08)
#define GPIO_DOUT     (0x0c)
#define GPIO_DOUTSET  (0x10)
#define GPIO_DOUTCLR  (0x14)
#define GPIO_DOUTTGL  (0x18)
#define GPIO_DIN      (0x1c)
#define GPIO_PINLOCKN (0x20)

#define GPIO_IRQ_EVEN 17
#define GPIO_IRQ_ODD 18

struct gamepad_dev {
	struct cdev cdev;
	struct class *class;
	dev_t dev_id;
	struct fasync_struct *async_queue;
	void *gpio_memory;
	/*
	   Flag to skip the first interrupt. As it is fired without any button
	   clicked, it is a false positive.
	 */
	int first_interrupt_handled;
};

static struct gamepad_dev device;

static int gamepad_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int gamepad_fasync(int fd, struct file *filp, int mode)
{
	return fasync_helper(fd, filp, mode, &device.async_queue);
}

static int gamepad_release(struct inode *inode, struct file *filp)
{
	gamepad_fasync(-1, filp, 0);
	return 0;
}

static ssize_t gamepad_read(struct file *filp, char __user * buff, size_t count,
			    loff_t * offp)
{
	unsigned long err;
	unsigned char button_state;
	unsigned int gpio_din;

	gpio_din =
	    ioread32((unsigned char *)device.gpio_memory + GPIO_PC_BASE +
		     GPIO_DIN);
	button_state = ~(gpio_din & 0xff);

	err = copy_to_user(buff, &button_state, 1);
	if (err != 0) {
		return -EFAULT;
	}

	return count == 1 ? 1 : 0;
}

static struct file_operations gamepad_fops = {
	.owner = THIS_MODULE,
	.open = gamepad_open,
	.fasync = gamepad_fasync,
	.release = gamepad_release,
	.read = gamepad_read,
};

static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
	unsigned int gpio_if;

	gpio_if = ioread32((unsigned char *)device.gpio_memory + GPIO_IF);
	iowrite32(gpio_if, (unsigned char *)device.gpio_memory + GPIO_IFC);

	if (!device.first_interrupt_handled) {
		device.first_interrupt_handled = 1;
		return IRQ_HANDLED;
	}

	kill_fasync(&device.async_queue, SIGIO, POLL_IN);

	return IRQ_HANDLED;
}

enum gamepad_error {
	ERR_ALLOC_CHRDEV,
	ERR_REQUEST_GPIO_MEM,
	ERR_CDEV_ADD,
	ERR_CLASS_CREATE,
	ERR_DEVICE_CREATE,
};

static void cleanup_error(enum gamepad_error error)
{
	switch (error) {
	case ERR_DEVICE_CREATE:
		class_destroy(device.class);

	case ERR_CLASS_CREATE:
		cdev_del(&device.cdev);

	case ERR_CDEV_ADD:
		iounmap(device.gpio_memory);
		release_mem_region(GPIO_BASE, GPIO_LENGTH);

	case ERR_REQUEST_GPIO_MEM:
		unregister_chrdev_region(device.dev_id, 1);

	case ERR_ALLOC_CHRDEV:
		break;
	}
}

static int __init gamepad_init(void)
{
	struct resource *gpio_memory_resource;
	struct device *chrdev;
	int err;

	memset(&device, 0, sizeof(struct gamepad_dev));

	// Initialize chrdev
	err = alloc_chrdev_region(&device.dev_id, 0, 1, MODNAME);
	if (err) {
		printk(KERN_WARNING MODNAME
		       ": Could not allocate chrdev for gamepad. (%i)\n", err);
		cleanup_error(ERR_ALLOC_CHRDEV);
		return err;
	}

	cdev_init(&device.cdev, &gamepad_fops);
	device.cdev.owner = THIS_MODULE;
	device.cdev.ops = &gamepad_fops;

	// Set up GPIO
	gpio_memory_resource =
	    request_mem_region(GPIO_BASE, GPIO_LENGTH, MODNAME);
	if (!gpio_memory_resource) {
		err = -1;
		printk(KERN_WARNING MODNAME
		       ": Could not allocate GPIO registers.\n");
		cleanup_error(ERR_REQUEST_GPIO_MEM);
		return err;
	}

	device.gpio_memory = ioremap_nocache(GPIO_BASE, GPIO_LENGTH);

	iowrite32(2,
		  (unsigned char *)device.gpio_memory + GPIO_PC_BASE +
		  GPIO_CTRL);
	iowrite32(0x33333333,
		  (unsigned char *)device.gpio_memory + GPIO_PC_BASE +
		  GPIO_MODEL);
	iowrite32(0xff,
		  (unsigned char *)device.gpio_memory + GPIO_PC_BASE +
		  GPIO_DOUT);

	iowrite32(0x22222222,
		  (unsigned char *)device.gpio_memory + GPIO_EXTIPSELL);
	iowrite32(0xff, (unsigned char *)device.gpio_memory + GPIO_EXTIFALL);

	err = cdev_add(&device.cdev, device.dev_id, 1);
	if (err) {
		printk(KERN_WARNING MODNAME ": Could not add gamepad. (%i)\n",
		       err);
		cleanup_error(ERR_CDEV_ADD);
		return err;
	}

	device.class = class_create(THIS_MODULE, MODNAME);
	if (IS_ERR(device.class)) {
		err = PTR_ERR(device.class);
		printk(KERN_WARNING MODNAME
		       ": Could not create class for the device. (%i)\n", err);
		cleanup_error(ERR_CLASS_CREATE);
		return err;
	}

	chrdev =
	    device_create(device.class, NULL, device.dev_id, NULL, MODNAME);
	if (IS_ERR(chrdev)) {
		err = PTR_ERR(chrdev);
		printk(KERN_WARNING MODNAME ": Could not create device. (%i)\n",
		       err);
		cleanup_error(ERR_DEVICE_CREATE);
		return err;
	}
	// Set up interrupt registers
	iowrite32(0xff, (unsigned char *)device.gpio_memory + GPIO_IEN);
	iowrite32(0, (unsigned char *)device.gpio_memory + GPIO_IF);
	iowrite32(0, (unsigned char *)device.gpio_memory + GPIO_IFC);

	err =
	    request_irq(GPIO_IRQ_EVEN, &gpio_irq_handler, 0, MODNAME, &device);
	if (err != 0) {
		printk(KERN_WARNING MODNAME
		       ": Could not register interrupt handler. (%i)\n", err);
	}

	err = request_irq(GPIO_IRQ_ODD, &gpio_irq_handler, 0, MODNAME, &device);
	if (err != 0) {
		printk(KERN_WARNING MODNAME
		       ": Could not register interrupt handler. (%i)\n", err);
	}

	printk(KERN_INFO MODNAME ": Initialized\n");

	return 0;
}

static void __exit gamepad_cleanup(void)
{
	// Unregister interrupt handlers
	free_irq(GPIO_IRQ_EVEN, &device);
	free_irq(GPIO_IRQ_ODD, &device);

	device_destroy(device.class, device.dev_id);
	class_destroy(device.class);
	cdev_del(&device.cdev);
	iounmap(device.gpio_memory);
	release_mem_region(GPIO_BASE, GPIO_LENGTH);
	unregister_chrdev_region(device.dev_id, 1);

	printk(KERN_INFO MODNAME ": Cleanup complete\n");
}

module_init(gamepad_init);
module_exit(gamepad_cleanup);

MODULE_DESCRIPTION("Gamepad driver.");
MODULE_LICENSE("GPL");
