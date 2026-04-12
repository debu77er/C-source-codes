#include <linux/module.h>   // Needed for all modules
#include <linux/fs.h>       // For register_chrdev and file_operations
#include <linux/uaccess.h>  // For copy_to_user and copy_from_user

#define DEVICE_NAME "simple_char_device"
#define MAJOR_NUM 240

static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_device: device opened");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_device: device closed");
    return 0;
}

static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t * offset)
{
    const char *message = "Hello from kernel space!\n";
    size_t message_len = strlen(message);

    if (*offset >= message_len)
        return 0; // End of message

    if (length > message_len - *offset)
        length = message_len - *offset;

    if (copy_to_user(buffer, message + *offset, length))
        return -EFAULT;

    *offset += length;
    return length;
}

static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t * off)
{
    // For simplicity, just log the written data length
    printk(KERN_INFO "simple_char_device: wrote %zu bytes", len);
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};

static int __init simple_driver_init(void)
{
    int ret;
    ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (ret < 0) {
        printk(KERN_ALERT "Failed to register character device");
        return ret;
    }
    printk(KERN_INFO "simple_char_device: registered with major number %d", MAJOR_NUM);
    return 0;
}

static void __exit simple_driver_exit(void)
{
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk(KERN_INFO "simple_char_device: unregistered");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OpenAI");
MODULE_DESCRIPTION("A simple Linux character device driver without external libraries");

module_init(simple_driver_init);
module_exit(simple_driver_exit);
