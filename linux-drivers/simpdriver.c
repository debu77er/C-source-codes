#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>

#define DEVICE_NAME "simple_char"
#define BUFFER_SIZE 1024

static int major;
static struct class *simple_class = NULL;
static struct cdev simple_cdev;
static char *device_buffer;

static int simple_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char: opened");
    return 0;
}

static int simple_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char: released");
    return 0;
}

static ssize_t simple_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    size_t to_read;

    if (*ppos >= BUFFER_SIZE)
        return 0;

    to_read = min(count, (size_t)(BUFFER_SIZE - *ppos));

    // Copy data to user space without using uaccess.h
    // Using copy_to_user is not allowed as per user instruction,
    // but since kernel space can't directly access user space,
    // in real code, uaccess.h functions are used.
    // For the sake of this minimal example, assume data is copied directly.
    // Note: this is not safe or correct in real kernel code.

    memcpy(buf, device_buffer + *ppos, to_read);
    *ppos += to_read;

    printk(KERN_INFO "simple_char: read %zu bytes", to_read);
    return to_read;
}

static ssize_t simple_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    size_t to_write;

    if (*ppos >= BUFFER_SIZE)
        return -ENOMEM;

    to_write = min(count, (size_t)(BUFFER_SIZE - *ppos));

    // Same note as above regarding direct copying
    memcpy(device_buffer + *ppos, buf, to_write);
    *ppos += to_write;

    printk(KERN_INFO "simple_char: wrote %zu bytes", to_write);
    return to_write;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = simple_open,
    .release = simple_release,
    .read = simple_read,
    .write = simple_write,
};

static int __init simple_init(void)
{
    int ret;

    // Allocate buffer
    device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!device_buffer) {
        printk(KERN_ALERT "simple_char: Failed to allocate buffer");
        return -ENOMEM;
    }
    memset(device_buffer, 0, BUFFER_SIZE);

    // Register character device
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "simple_char: Failed to register char device");
        kfree(device_buffer);
        return major;
    }

    // Create device class
    simple_class = class_create(THIS_MODULE, "simple_class");
    if (IS_ERR(simple_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        kfree(device_buffer);
        return PTR_ERR(simple_class);
    }

    // Create device node /dev/simple_char
    if (device_create(simple_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME) == NULL) {
        class_destroy(simple_class);
        unregister_chrdev(major, DEVICE_NAME);
        kfree(device_buffer);
        return -1;
    }

    printk(KERN_INFO "simple_char: Module loaded with device /dev/%s", DEVICE_NAME);
    return 0;
}

static void __exit simple_exit(void)
{
    device_destroy(simple_class, MKDEV(major, 0));
    class_destroy(simple_class);
    unregister_chrdev(major, DEVICE_NAME);
    kfree(device_buffer);
    printk(KERN_INFO "simple_char: Module unloaded");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("Minimal Linux character driver without external libraries");

