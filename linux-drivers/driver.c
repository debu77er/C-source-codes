#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>  // for copy_to_user and copy_from_user
#include <linux/device.h>

#define DEVICE_NAME "simple_device"
#define CLASS_NAME  "simple_class"
#define BUFFER_SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChatGPT");
MODULE_DESCRIPTION("A simple character device driver without external libraries");
MODULE_VERSION("0.1");

static int major_number;
static char message_buffer[BUFFER_SIZE] = {0};
static short message_size;
static struct class* simple_class = NULL;
static struct device* simple_device = NULL;

/* Function prototypes */
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

/* Open function */
static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "simple_device: Device opened");
    return 0;
}

/* Read function */
static ssize_t dev_read(struct file *filep, char *user_buffer, size_t len, loff_t *offset)
{
    size_t to_copy;

    if (*offset >= message_size)
        return 0; // EOF

    to_copy = min(len, (size_t)(message_size - *offset));

    if (copy_to_user(user_buffer, message_buffer + *offset, to_copy) != 0)
        return -EFAULT;

    *offset += to_copy;
    printk(KERN_INFO "simple_device: Sent %zu bytes to user", to_copy);
    return to_copy;
}

/* Write function */
static ssize_t dev_write(struct file *filep, const char *user_buffer, size_t len, loff_t *offset)
{
    size_t to_copy = min(len, (size_t)BUFFER_SIZE);

    if (copy_from_user(message_buffer, user_buffer, to_copy) != 0)
        return -EFAULT;

    message_size = to_copy;
    *offset = 0;

    printk(KERN_INFO "simple_device: Received %zu bytes from user", to_copy);
    return to_copy;
}

/* Release (close) function */
static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "simple_device: Device closed");
    return 0;
}

/* Module initialization */
static int __init simple_init(void)
{
    printk(KERN_INFO "simple_device: Initializing the driver");

    // Register a major number dynamically
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
    {
        printk(KERN_ALERT "Failed to register a major number");
        return major_number;
    }
    printk(KERN_INFO "Registered correctly with major number %d", major_number);

    // Register the device class
    simple_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(simple_class))
    {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class");
        return PTR_ERR(simple_class);
    }
    printk(KERN_INFO "Device class registered correctly");

    // Register the device driver
    simple_device = device_create(simple_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(simple_device))
    {
        class_destroy(simple_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device");
        return PTR_ERR(simple_device);
    }
    printk(KERN_INFO "Device created correctly");

    return 0;
}

/* Module cleanup */
static void __exit simple_exit(void)
{
    device_destroy(simple_class, MKDEV(major_number, 0));
    class_unregister(simple_class);
    class_destroy(simple_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "simple_device: Driver unloaded");
}

module_init(simple_init);
module_exit(simple_exit);
